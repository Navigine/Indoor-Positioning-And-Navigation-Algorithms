package com.navigine.navigine;
import com.navigine.navigine.*;
import com.navigine.naviginesdk.*;

import android.app.*;
import android.bluetooth.*;
import android.content.*;
import android.hardware.*;
import android.os.*;
import android.util.*;
import android.widget.*;
import java.io.*;
import java.lang.*;
import java.nio.*;
import java.nio.channels.*;
import java.net.*;
import java.util.*;

public class IMUThread extends Thread
{
  public static final String  TAG           = "Navigine.IMU";
  
  // State constants
  public static final int STATE_STOP        = -100; // Stop state
  public static final int STATE_IDLE        =  100; // Idle state
  public static final int STATE_CONNECT     =  200; // Sending messages
  public static final int STATE_DISCONNECT  =  300; // Sending messages
  public static final int STATE_NORMAL      =  400; // Sending messages
  
  private int mState     = STATE_IDLE;
  private int mStateTmp  = 0; 
  
  private BluetoothSocket mIMUSocket = null;
  private InputStream mIMUInputStream = null;
  private OutputStream mIMUOutputStream = null;
  private float[] mIMUValues = new float[4];
  private int mLocId = 0;
  private int mSubLocId = 0;
  private float mX0 = 0, mY0 = 0, mA0 = 0;
  private int mPacketNumber = -1;
  private long mPacketTime  = -1;
  
  private String mLogFile = null;
  
  public IMUThread()
  {
    super.start();
  }
  
  public synchronized void terminate()
  {
    mStateTmp = STATE_STOP;
  }
  
  public synchronized void setLogFile(String logFile)
  {
    mLogFile = logFile;
  }
  
  public synchronized void connect(int loc, int subLoc, float x0, float y0, float a0)
  {
    if (mState != STATE_IDLE)
    {
      Log.e(TAG, "IMU-thread is not in the IDLE state! Ignoring connect request!");
      return;
    }
    mStateTmp = STATE_CONNECT;
    mLocId = loc;
    mSubLocId = subLoc;
    mX0 = x0;
    mY0 = y0;
    mA0 = a0;
    mPacketNumber = -1;
    mPacketTime   = -1;
  }
  
  public synchronized void disconnect()
  {
    if (mState != STATE_NORMAL)
    {
      Log.e(TAG, "IMU-thread is not in the NORMAL state! Ignoring connect request!");
      return;
    }
    
    mStateTmp = STATE_DISCONNECT;
    mPacketNumber = -1;
    mPacketTime   = -1;
  }
  
  public synchronized int getConnectionState()
  {
    return mState;
  }
  
  private static String byteArrayToHex(byte[] a)
  {
    StringBuilder sb = new StringBuilder(a.length * 2);
    for(int i = 0; i < a.length; ++i)
      sb.append(String.format("%02X", a[i] & 0xff));
    return sb.toString();
  }
  
  private void integrate(float dx, float dy, float da)
  {
    mIMUValues[0] += (float)(-dx * Math.cos(mIMUValues[3]) + dy * Math.sin(mIMUValues[3]));
    mIMUValues[1] += (float)(+dx * Math.sin(mIMUValues[3]) + dy * Math.cos(mIMUValues[3]));
    mIMUValues[3] += da;
    
    if (mIMUValues[3] > Math.PI)
      mIMUValues[3] -= 2 * Math.PI;
    if (mIMUValues[3] < -Math.PI)
      mIMUValues[3] += 2 * Math.PI;
    
    Log.d(TAG, String.format(Locale.ENGLISH, "IMU difference: [dx=%.4f, dy=%.4f, da=%.4f]", dx, dy, da));
    Log.d(TAG, String.format(Locale.ENGLISH, "IMU coordinates: (%.2f, %.2f, %.2f, %.2f)",
          mIMUValues[0], mIMUValues[1], mIMUValues[2], mIMUValues[3]));
    
    logMessage();
  }
  
  private void logMessage()
  {
    String logFile = null;
    synchronized (this)
    {
      if (mLogFile != null)
        logFile = new String(mLogFile);
    }
    
    if (logFile == null || logFile.length() == 0)
      return;
    
    String deviceId = "";
    String deviceName = "";
    String macAddress = "";
    
    if (NavigineApp.Navigation != null)
    {
      deviceId   = NavigineApp.Navigation.getDeviceId();
      deviceName = NavigineApp.Navigation.getDeviceName();
      macAddress = NavigineApp.Navigation.getMacAddress();
    }
    
    long timeNow = DateTimeUtils.currentTimeMillis();
    //String message = String.format(Locale.ENGLISH, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
    //                                               "<message request=\"redirect\" packetNumber=\"%d\">\n" +
    //                                               "  <device type=\"android\" model=\"%s\" id=\"%s\" address=\"%s\" time=\"%s\"/>\n" +
    //                                               "  <coordinates location=\"%d\" sublocation=\"%d\" x=\"%.4f\" y=\"%.4f\" z=\"%.4f\" r=\"%.4f\"/>\n" +
    //                                               "  <orientation azimuth=\"%.4f\" pitch=\"%.4f\" roll=\"%.4f\"/>\n" +
    //                                               "</message>\n",
    //                                               mPacketNumber,
    //                                               deviceName, deviceId, macAddress, DateTimeUtils.currentDate(timeNow),
    //                                               mLocId, mSubLocId, mX0 + mIMUValues[0], mY0 + mIMUValues[1], 0.0f, 1.0f,
    //                                               mA0 + mIMUValues[3], 0.0f, 0.0f);
    
    String message = String.format(Locale.ENGLISH, "%s %d %d %.2f %.2f %.2f %.2f %.2f",
                                   DateTimeUtils.currentDate(timeNow), mLocId, mSubLocId,
                                   mX0 + mIMUValues[0], mY0 + mIMUValues[1], 0.0f, 1.0f,
                                   mA0 + mIMUValues[3]);
    
    try
    {
      FileWriter fw = new FileWriter(logFile, true);
      fw.write(message);
      fw.write("\n\n");
      fw.close();
    }
    catch (Throwable e)
    {
      Log.e(TAG, Log.getStackTraceString(e));
      Log.e(TAG, "Unable to open/write output log file " + logFile);
    }
  }
  
  public boolean connectToIMU()
  {
    if (mIMUSocket != null)
      return true;
    
    //final String IMU_ADDRESS = "00:80:E1:B2:EA:4C";
    final UUID SERIAL_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); //UUID for serial connection
    
    BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    if (bluetoothAdapter == null)
      return false;
    
    BluetoothDevice device = null; //bluetoothAdapter.getRemoteDevice(IMU_ADDRESS);
    
    try
    {
      List<BluetoothDevice> devices = new ArrayList<BluetoothDevice>(bluetoothAdapter.getBondedDevices());
      if (devices.isEmpty())
      {
        Log.d(TAG, "IMU-thread: no bonded devices found!");
        return false;
      }
      else
        device = devices.get(0);
    }
    catch (Throwable e)
    {
      return false;
    }
    
    Log.d(TAG, "Connecting IMU-device: " + device.toString());
    if (device == null)
      return false;
    
    // Get a BluetoothSocket to connect with the given BluetoothDevice
    mIMUSocket = null;
    try
    {
      mIMUSocket = device.createRfcommSocketToServiceRecord(SERIAL_UUID); 
    }
    catch (IOException e)
    {
      return false;
    }
    
    try
    {
      mIMUSocket.connect(); 
      mIMUInputStream  = mIMUSocket.getInputStream();
      mIMUOutputStream = mIMUSocket.getOutputStream();
      Log.d(TAG, "IMU-device connected!");
      
      Log.d(TAG, "IMU-device: sending start command [0x14 0x00 0x14]");
      mIMUValues[0] = mIMUValues[1] = mIMUValues[2] = mIMUValues[3] = 0.0f;
      mIMUOutputStream.write(0x14);
      mIMUOutputStream.write(0x00);
      mIMUOutputStream.write(0x14);
      mIMUOutputStream.flush();
      
      byte[] data = new byte[68];
      int count = 0;
      
      Log.d(TAG, "IMU-device: receiving 68-byte packet");
      while (count < data.length)
      {
        int result = mIMUInputStream.read(data, count, data.length - count);
        //Log.d(TAG, String.format(Locale.ENGLISH, "count = %d", count));
        count += result;
      }
      
      Log.d(TAG, byteArrayToHex(data));
      
      int check_sum = (data[62] & 0xff) * 256 + (data[63] & 0xff);
      int sum1 = 0;
      for(int j = 0; j < 62; ++j)
        sum1 += data[j] & 0xff;
      
      Log.d(TAG, String.format(Locale.ENGLISH, "Check sum: %d", sum1));
      
      if (check_sum == sum1)
        Log.d(TAG, "Check sum correct!");
      else
        Log.d(TAG, "Check sum incorrect!");
      
      float dx = ByteBuffer.wrap(data, 4, 4).order(ByteOrder.BIG_ENDIAN).getFloat();
      float dy = ByteBuffer.wrap(data, 8, 4).order(ByteOrder.BIG_ENDIAN).getFloat();
      float da = ByteBuffer.wrap(data, 16, 4).order(ByteOrder.BIG_ENDIAN).getFloat();
      
      int packetNumber = (data[1] & 0xff) * 256 + (data[2] & 0xff);
      if (packetNumber != mPacketNumber)
      {
        integrate(dx, dy, da);
        mPacketNumber = packetNumber;
        mPacketTime   = DateTimeUtils.currentTimeMillis();
      }
      
      // Sending acknowledgement
      int p1 = ((int)data[1] + 256) % 256;
      int p2 = ((int)data[2] + 256) % 256;
      
      byte[] ack = new byte[5];
      ack[0] = 0x01;
      ack[1] = (byte)p1;
      ack[2] = (byte)p2;
      ack[3] = (byte)((p1 + p2 + 1) / 256);
      ack[4] = (byte)((p1 + p2 + 1) % 256);
      
      Log.d(TAG, String.format(Locale.ENGLISH, "Sending acknowledgement: %s", byteArrayToHex(ack)));
      mIMUOutputStream.write(ack);
      mIMUOutputStream.flush();
    }
    catch (Throwable e)
    {
      return false;
    }
    
    return true;
  }
  
  public void updateIMU()
  {
    if (mIMUSocket == null || mIMUInputStream == null || mIMUOutputStream == null)
      return;
    
    try
    {
      byte[] data = new byte[64];
      int count = 0;
      
      Log.d(TAG, "IMU-device: receiving 64-byte packet");
      while (count < data.length)
      {
        int result = mIMUInputStream.read(data, count, data.length - count);
        //Log.d(TAG, String.format(Locale.ENGLISH, "count = %d", count));
        
        count += result;
      }
      
      Log.d(TAG, byteArrayToHex(data));
      
      int check_sum = (data[62] & 0xff) * 256 + (data[63] & 0xff);
      int sum1 = 0;
      for(int j = 0; j < 62; ++j)
        sum1 += data[j] & 0xff;
      
      Log.d(TAG, String.format(Locale.ENGLISH, "Check sum: %d", sum1));
      
      if (check_sum == sum1)
        Log.d(TAG, "Check sum correct!");
      else
        Log.d(TAG, "Check sum incorrect!");
      
      float dx = ByteBuffer.wrap(data, 4, 4).order(ByteOrder.BIG_ENDIAN).getFloat();
      float dy = ByteBuffer.wrap(data, 8, 4).order(ByteOrder.BIG_ENDIAN).getFloat();
      float da = ByteBuffer.wrap(data, 16, 4).order(ByteOrder.BIG_ENDIAN).getFloat();
      
      int packetNumber = (data[1] & 0xff) * 256 + (data[2] & 0xff);
      if (packetNumber != mPacketNumber)
      {
        integrate(dx, dy, da);
        mPacketNumber = packetNumber;
        mPacketTime   = DateTimeUtils.currentTimeMillis();
      }
      
      // Sending acknowledgement
      int p1 = ((int)data[1] + 256) % 256;
      int p2 = ((int)data[2] + 256) % 256;
      
      byte[] ack = new byte[5];
      ack[0] = 0x01;
      ack[1] = (byte)p1;
      ack[2] = (byte)p2;
      ack[3] = (byte)((p1 + p2 + 1) / 256);
      ack[4] = (byte)((p1 + p2 + 1) % 256);
      
      Log.d(TAG, String.format(Locale.ENGLISH, "Sending acknowledgement: %s", byteArrayToHex(ack)));
      mIMUOutputStream.write(ack);
      mIMUOutputStream.flush();
    }
    catch (Throwable e) {}
  }
    
  public void disconnectFromIMU()
  {
    if (mIMUSocket == null)
      return;
    
    try
    {
      mIMUOutputStream.write(0x27);
      mIMUOutputStream.write(0x00);
      mIMUOutputStream.write(0x27);
      mIMUOutputStream.flush();
      
      mIMUOutputStream.write(0x21);
      mIMUOutputStream.write(0x00);
      mIMUOutputStream.write(0x21);
      mIMUOutputStream.flush();
      mIMUSocket.close();
      
      mIMUSocket = null;
      mIMUInputStream = null;
      mIMUOutputStream = null;
      mIMUValues[0] = mIMUValues[1] = mIMUValues[2] = mIMUValues[3] = 0.0f;
    }
    catch (Throwable e) {}
  }
  
  public synchronized DeviceInfo getDevice()
  {
    long timeNow = DateTimeUtils.currentTimeMillis();
    if (Math.abs(mPacketTime - timeNow) > 6000)
      return null;
    
    DeviceInfo deviceInfo = new DeviceInfo();
    deviceInfo.id            = NavigineApp.Navigation.getDeviceId();
    deviceInfo.type          = "android";
    deviceInfo.time          = DateTimeUtils.currentDate(timeNow);
    deviceInfo.index         = 0;
    deviceInfo.location      = mLocId;
    deviceInfo.subLocation   = mSubLocId;
    deviceInfo.x             = mX0 + mIMUValues[0]; // in meters
    deviceInfo.y             = mY0 + mIMUValues[1]; // in meters
    deviceInfo.z             = 0.0f; // in meters
    deviceInfo.r             = 1.0f; // in meters
    deviceInfo.azimuth       = mA0 + mIMUValues[3];  // in degrees
    deviceInfo.pitch         = 0.0f; // in degrees
    deviceInfo.roll          = 0.0f; // in degrees
    deviceInfo.timeLabel     = timeNow; // in milliseconds
    return deviceInfo;
  }
  
  @Override public void run()
  {
    Looper.prepare();
    int state = STATE_IDLE;
    
    while (true)
    {
      // Reading shared state variables
      synchronized(this)
      {
        if (mStateTmp != 0)
        {
          if (mStateTmp == STATE_STOP)
          {
            return;
          }
          state = mStateTmp;
          mStateTmp = 0;
        }
        mState = state;
      }
      
      switch (state)
      {
        case STATE_IDLE:
          //Log.d(TAG, "IDLE");
          break;
        
        case STATE_CONNECT:
          //Log.d(TAG, "CONNECT");
          if (connectToIMU())
          {
            state = STATE_NORMAL;
          }
          else
          {
            state = STATE_IDLE;
          }
          break;
        
        case STATE_DISCONNECT:
          //Log.d(TAG, "DISCONNECT");
          disconnectFromIMU();
          state = STATE_IDLE;
          break;
        
        case STATE_NORMAL:
          //Log.d(TAG, "NORMAL");
          updateIMU();
          break;
      } // switch
      
      try
      {
        Thread.sleep(100);
      }
      catch (Throwable e)
      {
        Log.e(TAG, Log.getStackTraceString(e));
      }
    } // end of loop
  } // end of run()
}

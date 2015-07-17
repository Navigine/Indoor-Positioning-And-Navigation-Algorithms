package com.navigine.navigine;
import com.navigine.navigine.*;
import com.navigine.naviginesdk.*;

import android.app.*;
import android.content.*;
import android.hardware.*;
import android.os.*;
import android.util.*;
import java.io.*;
import java.lang.*;
import java.nio.*;
import java.nio.channels.*;
import java.net.*;
import java.util.*;

public class SenderThread extends Thread
{
  public static final String  TAG             = "Navigine.Sender";
  public static final String  SIGNATURE       = "WScanner"; // Requires 8 symbols
  public static final String  DEFAULT_HOST    = "192.168.0.1";
  public static final int     DEFAULT_PORT    = 27015;
  public static final int     DEFAULT_FREQ    = 10;
  public static final int     HEADER_LENGTH   = SIGNATURE.length() + 4;
  public static final int     MESSAGES_MAX    = 2;
  
  public static final int     CONNECT_TIMEOUT = 5000; // Connection timeout (milliseconds)
  public static final int     SEND_TIMEOUT    = 5000; // Send timeout (milliseconds)
  
  // State constants
  public static final int     STATE_STOP      = -100; // Stop state
  public static final int     STATE_IDLE      =  100; // Idle state
  public static final int     STATE_RESOLVE   =  190; // Resolving hostname
  public static final int     STATE_CONNECT   =  200; // Connecting to server
  public static final int     STATE_RECONNECT =  300; // Reconnecting to server
  public static final int     STATE_NORMAL    =  400; // Sending messages
  
  private String  mHost      = DEFAULT_HOST;
  private int     mPort      = DEFAULT_PORT;
  private int     mFreq      = DEFAULT_FREQ;
  private int     mState     = STATE_IDLE;
  private int     mStateTmp  = 0; 
  private String  mError     = "";
  
  private long mScanTime     = 0;
  private long mSensorTime   = 0;
  private long mFromTime     = 0;
  private int mTotalBytes    = 0;
  private int mTotalMessages = 0;
  private int mLostMessages  = 0;
  private int mSendMessages  = 0;
  private int mPacketNumber  = 0;
  private List<String> mMessages = new ArrayList<String>();
  
  public SenderThread()
  {
    super.start();
  }
  
  public synchronized void terminate()
  {
    mStateTmp = STATE_STOP;
  }
  
  public synchronized void idle()
  {
    mStateTmp = STATE_IDLE;
  }
  
  public synchronized void reconnect(String host, int port, int freq)
  {
    mHost = new String(host);
    mPort = port;
    mFreq = freq;
    mStateTmp = STATE_RECONNECT;
    mFromTime = DateTimeUtils.currentTimeMillis();
    mTotalBytes = 0;
    mTotalMessages = 0;
    mLostMessages  = 0;
    mSendMessages  = 0;
  }
  
  public synchronized int getConnectionState()
  {
    return mState;
  }
  
  public synchronized int getTotalTime()      { return (int)((DateTimeUtils.currentTimeMillis() - mFromTime) / 1000); }
  public synchronized int getTotalBytes()     { return mTotalBytes;       }
  public synchronized int getTotalMessages()  { return mTotalMessages;    }
  public synchronized int getLostMessages()   { return mLostMessages;     }
  public synchronized int getSendMessages()   { return mSendMessages;     }
  public synchronized int getBufMessages()    { return mMessages.size();  }
  
  public synchronized void addMessage(String message)
  {
    if (mMessages.size() >= MESSAGES_MAX)
    {
      mMessages.remove(0);
      mLostMessages += 1;
    }
    mMessages.add(message);
    mTotalMessages += 1;
  }
  
  public void postMessage(long timeNow)
  {
    String message = NavigineApp.Navigation.buildMessage(timeNow, ++mPacketNumber,
                                                         NavigineApp.Navigation.getScanResults(mScanTime),
                                                         NavigineApp.Navigation.getSensorResults(mSensorTime));
    mScanTime = mSensorTime = timeNow + 1;
    addMessage(message);
  }
  
  public void postMessageRedirect(long timeNow)
  {
    StringBuilder messageBuilder = new StringBuilder();
    messageBuilder.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    messageBuilder.append(String.format(Locale.ENGLISH, "<message request=\"redirect\" packetNumber=\"%d\">\n",
                                        ++mPacketNumber));
    
    String deviceId   = NavigineApp.Navigation.getDeviceId();
    String deviceName = NavigineApp.Navigation.getDeviceName();
    String macAddress = NavigineApp.Navigation.getMacAddress();
    
    List<DeviceInfo> devices = NavigineApp.Navigation.getDeviceList();
    if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_NORMAL)
    {
      devices = new ArrayList<DeviceInfo>();
      DeviceInfo device = NavigineApp.IMU.getDevice();
      if (device != null)
        devices.add(device);
    }
    
    if (devices == null || devices.isEmpty())
      return;
    
    DeviceInfo info = devices.get(0);
    
    messageBuilder.append(String.format(Locale.ENGLISH, "  <device type=\"android\" model=\"%s\" id=\"%s\" " +
                                                        "address=\"%s\" time=\"%s\"/>\n",
                                                         deviceName, deviceId, macAddress,
                                                         DateTimeUtils.currentDate(timeNow)));
    
    messageBuilder.append(String.format(Locale.ENGLISH, "  <coordinates location=\"%d\" sublocation=\"%d\" " +
                                                        "x=\"%.4f\" y=\"%.4f\" z=\"%.4f\" r=\"%.4f\"/>\n",
                                                        info.location, info.subLocation,
                                                        info.x, info.y, info.z, info.r));
    
    messageBuilder.append(String.format(Locale.ENGLISH, "  <orientation azimuth=\"%.4f\" pitch=\"%.4f\" roll=\"%.4f\"/>\n",
                                                        info.azimuth, info.pitch, info.roll));
    
    messageBuilder.append("</message>\n");
    // Adding message to the Sender queue
    addMessage(messageBuilder.toString());
  }
  
  private void closeChannel(SocketChannel channel)
  {
    // Trying to shutdown input
    try
    {
      if (channel != null)
      {
        channel.socket().shutdownInput();
      }
    }
    catch (Throwable e)
    {
      //Log.e(TAG, Log.getStackTraceString(e));
    }
    
    // Trying to shutdown output
    try
    {
      if (channel != null)
      {
        channel.socket().shutdownOutput();
      }
    }
    catch (Throwable e)
    {
      //Log.e(TAG, Log.getStackTraceString(e));
    }
    
    // Trying to close
    try
    {
      if (channel != null)
        channel.close();
    }
    catch (Throwable e)
    {
      //Log.e(TAG, "Close error!");
      //Log.e(TAG, Log.getStackTraceString(e));
    }
  }
  
  @Override public void run()
  {
    // State parameters
    String  host      = DEFAULT_HOST;
    int     port      = DEFAULT_PORT;
    int     freq      = DEFAULT_FREQ;
    int     state     = STATE_IDLE;
    String  error     = "";
    
    // Time parameters
    long timeNow      = DateTimeUtils.currentTimeMillis();
    long connectTime  = timeNow;
    long sendTime     = timeNow;
    long postTime     = timeNow;
    
    // Connection parameters
    SocketChannel channel = null;
    ByteBuffer sendBuffer = null;
    
    InetAddress hostAddress = null;
    DnsResolver dnsResolver = null;
    Thread      dnsThread   = null;
    
    while (true)
    {
      boolean shouldWait = true;
      timeNow = DateTimeUtils.currentTimeMillis();
      
      //Log.d(TAG, "Running...");
      
      // Reading shared state variables
      synchronized(this)
      {
        host = new String(mHost);
        port = mPort;
        freq = Math.max(Math.min(mFreq, 100), 1);
        
        if (mStateTmp != 0)
        {
          if (mStateTmp == STATE_STOP)
          {
            closeChannel(channel);
            channel = null;
            return;
          }
          state = mStateTmp;
          mStateTmp = 0;
        }
        mState = state;
        
        if (error != null && error.length() > 0)
        {
          mError = new String(error);
          error   = "";
        }
      }
      
      switch (state)
      {
        case STATE_IDLE:
          closeChannel(channel);
          hostAddress = null;
          dnsResolver = null;
          dnsThread = null;
          channel = null;
          break;
          
        case STATE_RECONNECT:
          //Log.d(TAG, String.format(Locale.ENGLISH, "Reconnecting to server %s:%d...", host, port));
          closeChannel(channel);
          hostAddress = null;
          dnsResolver = null;
          dnsThread = null;
          channel = null;
          state = STATE_RESOLVE;
          connectTime = timeNow;
          break;
        
        case STATE_RESOLVE:
          if (timeNow > connectTime + CONNECT_TIMEOUT)
          {
            Log.d(TAG, String.format(Locale.ENGLISH, "Unable to connect to server %s:%d, reconnecting...", host, port));
            state = STATE_RECONNECT;
          }
          
          try
          {
            if (dnsThread == null || dnsResolver == null)
            {
              dnsResolver = new DnsResolver(host);
              dnsThread = new Thread(dnsResolver);
              dnsThread.start();
              break;
            }
            else
            {
              if (dnsThread.isAlive())
                break;
              
              hostAddress = dnsResolver.getAddress();
            }
            
            channel = SocketChannel.open();
            channel.configureBlocking(false);
            channel.socket().setTcpNoDelay(true);
            channel.socket().setSendBufferSize(1024);
            channel.connect(new InetSocketAddress(hostAddress.getHostAddress(), port));
            state = STATE_CONNECT;
          }
          catch (Throwable e)
          {
            //Log.e(TAG, "Connection error!");
            //Log.e(TAG, Log.getStackTraceString(e));
          }
          break;
        
        case STATE_CONNECT:
          if (timeNow > connectTime + CONNECT_TIMEOUT)
          {
            Log.d(TAG, String.format(Locale.ENGLISH, "Unable to connect to server %s:%d, reconnecting...", host, port));
            state = STATE_RECONNECT;
          }
          
          if (channel == null)
          {
            state = STATE_RECONNECT;
            break;
          }
          
          try
          {
            if (channel.finishConnect())
            {
              Log.d(TAG, "The connection has been established");
              state = STATE_NORMAL;
              sendTime = timeNow;
              sendBuffer = null;
            }
            break;
          }
          catch (Throwable e)
          {
            //Log.e(TAG, "Connection error!");
            //Log.e(TAG, Log.getStackTraceString(e));
            //error = String.format(Locale.ENGLISH, "Unable to connect to %s:%d!\n" +
            //                    "Please, check connection parameters " +
            //                    "and network settings!",
            //                    host, port);
            //Log.e(TAG, error);
            state = STATE_RECONNECT;
          }
          break;
          
        case STATE_NORMAL:
          if (channel == null)
          {
            state = STATE_RECONNECT;
            break;
          }
          
          if (Math.abs(timeNow - postTime) > 1000 / freq)
          {
            postTime = timeNow;
            //postMessage(timeNow);
            postMessageRedirect(timeNow);
          }
          
          if (Math.abs(timeNow - sendTime) > SEND_TIMEOUT)
          {
            // Unable to send a message for too long
            Log.d(TAG, String.format(Locale.ENGLISH, "Unable to send data for more than %d seconds!", SEND_TIMEOUT/1000));
            Log.d(TAG, String.format(Locale.ENGLISH, "Reconnecting to server %s:%d...", host, port));
            state = STATE_RECONNECT;
            break;
          }
          
          // No unfinished jobs
          if (sendBuffer == null)
          {
            // Copying messages to the local list
            String message = null;
            synchronized(this)
            {
              if (mMessages.size() > 0)
              {
                message = new String(mMessages.get(mMessages.size() - 1));
                //mMessages.remove(0); // Sending message from queue
                mMessages.clear();
              }
              else if (Math.abs(timeNow - sendTime) > 1000)
                message = ""; // Sending empty message
            }
            
            // No messages to send
            if (message == null)
              break;
            
            Log.d(TAG, "Send message: " + message);
            
            // Calculating message size (in bytes) and preparing buffer
            int size = HEADER_LENGTH + message.getBytes().length;
            sendBuffer = ByteBuffer.wrap(new byte[size]);
            sendBuffer.order(ByteOrder.LITTLE_ENDIAN);
            
            // Initializing array with zeros
            Arrays.fill(sendBuffer.array(), (byte)0);
            
            // Writing signature (8 bytes)
            String signature = SIGNATURE;
            sendBuffer.put(signature.getBytes());
            
            // Writing data size (4 bytes)
            sendBuffer.putInt(size);
            
            // Writing message string
            sendBuffer.put(message.getBytes());
            sendBuffer.rewind();
          }
          
          // Sending messages to server
          try
          {
            int count = channel.write(sendBuffer);
            if (count < 0)
            {
              error = String.format(Locale.ENGLISH, "Unable to send data to %s:%d!", host, port);
              state = STATE_RECONNECT;
              Log.e(TAG, error);
              break;
            }
            if (count == 0)
              break;
            
            sendTime = timeNow;
            synchronized(this)
            {
              mTotalBytes += count;
            }
            
            if (sendBuffer.hasRemaining())
              break;
            
            synchronized(this)
            {
              if (mMessages.size() > 0)
                shouldWait = false;
              mSendMessages += 1;
            }
            sendBuffer = null;
          }
          catch (Throwable e)
          {
            error = String.format(Locale.ENGLISH, "Unable to send data to %s:%d!", host, port);
            state = STATE_RECONNECT;
          }
          break;
      } // switch
      
      try
      {
        if (state != STATE_NORMAL || shouldWait)
          Thread.sleep(5);
      }
      catch (Throwable e)
      {
        Log.e(TAG, Log.getStackTraceString(e));
      }
    } // end of loop
  } // end of run()
}

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

public class ReceiverThread extends Thread
{
  public static final String  TAG             = "Navigine.Receiver";
  public static final String  SIGNATURE       = "WScanner"; // Requires 8 symbols
  public static final String  DEFAULT_HOST    = "192.168.0.1";
  public static final int     DEFAULT_PORT    = 27016;
  public static final int     HEADER_LENGTH   = SIGNATURE.length() + 4;
    
  public static final int     CONNECT_TIMEOUT = 10000;   // Connection timeout in milliseconds
  public static final int     RECEIVE_TIMEOUT = 10000;   // Receive timeout in milliseconds
  
  // State constants
  public static final int     STATE_STOP      = -100; // Stop state
  public static final int     STATE_IDLE      =  100; // Idle state
  public static final int     STATE_RESOLVE   =  190; // Resolving hostname
  public static final int     STATE_CONNECT   =  200; // Connecting to server
  public static final int     STATE_RECONNECT =  300; // Reconnecting to server
  public static final int     STATE_NORMAL    =  400; // Sending messages
  
  private String  mHost      = DEFAULT_HOST;
  private int     mPort      = DEFAULT_PORT;
  private int     mState     = STATE_IDLE;
  private int     mStateTmp  = 0; 
  private String  mError     = "";
  
  public ReceiverThread()
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
  
  public synchronized void reconnect(String host, int port)
  {
    mHost = new String(host);
    mPort = port;
    mStateTmp = STATE_RECONNECT;
  }
  
  public synchronized int getConnectionState()
  {
    return mState;
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
    String  host        = DEFAULT_HOST;
    int     port        = DEFAULT_PORT;
    int     state       = STATE_IDLE;
    long    timeNow     = DateTimeUtils.currentTimeMillis();
    long    recvTime    = timeNow;
    long    connectTime = timeNow;
    String  error       = "";
    
    // Connection parameters
    SocketChannel channel = null;
    ByteBuffer recvBuffer = null;
    
    InetAddress hostAddress = null;
    DnsResolver dnsResolver = null;
    Thread      dnsThread   = null;
    
    while (true)
    {
      boolean shouldWait = true;
      timeNow = DateTimeUtils.currentTimeMillis();
      
      // Reading shared state variables
      synchronized(this)
      {
        host = new String(mHost);
        port = mPort;
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
          break; // case STATE_IDLE
        
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
              recvTime = timeNow;
              recvBuffer = null;
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
          break; // case STATE_CONNECT
          
        case STATE_NORMAL:
          if (channel == null)
          {
            state = STATE_RECONNECT;
            break;
          }
          
          if (Math.abs(timeNow - recvTime) > RECEIVE_TIMEOUT)
          {
            // Unable to receive a message for too long
            // Probably, remote sender has dropped the connection
            Log.d(TAG, String.format(Locale.ENGLISH, "Unable to receive data for more than %d seconds!", RECEIVE_TIMEOUT/1000));
            Log.d(TAG, String.format(Locale.ENGLISH, "Reconnecting to server %s:%d...", host, port));
            state = STATE_RECONNECT;
            break;
          }
          
          if (recvBuffer == null)
          {
            recvBuffer = ByteBuffer.wrap(new byte[HEADER_LENGTH]);
            recvBuffer.order(ByteOrder.LITTLE_ENDIAN);
          }
          
          //Log.d(TAG, String.format(Locale.ENGLISH, "RecvBuffer: position=%d, limit=%d, remaining=%d",
          //    recvBuffer.position(), recvBuffer.limit(), recvBuffer.remaining()));
          
          try
          {
            int count = channel.read(recvBuffer);
            //Log.d(TAG, String.format(Locale.ENGLISH, "count = %d", count));
            
            if (count < 0)
            {
              error = String.format(Locale.ENGLISH, "Unable to receive data from %s:%d!", host, port);
              state = STATE_RECONNECT;
              Log.e(TAG, error);
              break;
            }
            if (count == 0)
              break;
            
            recvTime = timeNow;
            if (recvBuffer.hasRemaining())
              break;
          }
          catch (Throwable e)
          {
            Log.e(TAG, Log.getStackTraceString(e));
            error = String.format(Locale.ENGLISH, "Unable to receive data from %s:%d!", host, port);
            state = STATE_RECONNECT;
            Log.e(TAG, error);
            break;
          }
          
          shouldWait = false;
          
          if (recvBuffer.limit() < HEADER_LENGTH)
          {
            error = String.format(Locale.ENGLISH, "Invalid buffer size (%d)!", recvBuffer.limit());
            state = STATE_RECONNECT;
            Log.e(TAG, error);
            break;
          }
          
          if (recvBuffer.limit() == HEADER_LENGTH)
          {
            // Parsing message header
            // Verifying signature
            String signature = new String(recvBuffer.array(), 0, SIGNATURE.length());
            if (!signature.equals(SIGNATURE))
            {
              error = String.format(Locale.ENGLISH, "Error in signature (%s)!", signature);
              state = STATE_RECONNECT;
              Log.e(TAG, error);
              break;
            }
            
            // Reading packet size
            int totalSize = recvBuffer.getInt(SIGNATURE.length());
            if (totalSize < HEADER_LENGTH)
            {
              error = String.format(Locale.ENGLISH, "Invalid packet size (%d)!", totalSize);
              state = STATE_RECONNECT;
              Log.e(TAG, error);
              break;
            }
            if (totalSize == HEADER_LENGTH)
            {
              // Ignoring "empty" packets from server
              recvBuffer = null;
              break;
            }
            recvBuffer = ByteBuffer.wrap(new byte[totalSize - HEADER_LENGTH]);
            recvBuffer.order(ByteOrder.LITTLE_ENDIAN);
            break;
          }
          
          if (recvBuffer.limit() > HEADER_LENGTH)
          {
            // Parsing message body
            String message = new String(recvBuffer.array());
            NavigineApp.Navigation.parseMessage(message);
            recvBuffer = null;
            break;
          }
          break; // case STATE_NORMAL
      } // switch
      
      try
      {
        if (state != STATE_NORMAL || shouldWait)
          Thread.sleep(5);
      }
      catch (Throwable e)
      {
        Log.e(TAG, Log.getStackTraceString(e));
      } // end of loop
    }
  } // end of run()
}

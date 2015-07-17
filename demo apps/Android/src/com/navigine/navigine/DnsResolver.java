package com.navigine.navigine;
import com.navigine.navigine.*;

import android.app.*;
import android.content.*;
import android.hardware.*;
import android.os.*;
import android.util.*;
import java.io.*;
import java.lang.*;
import java.nio.*;
import java.net.*;
import java.util.*;

public class DnsResolver implements Runnable
{
  private static final String TAG = "Navigine.DnsResolver";
  private InetAddress mInetAddress = null;
  private String mHostName = null;
  
  public DnsResolver(String hostName)
  {
    mHostName = hostName;
  }
  
  public void run()
  {
    try
    {
      InetAddress addr = InetAddress.getByName(mHostName);
      //Log.d(TAG, String.format(Locale.ENGLISH, "Resolved '%s' to %s", mHostName, addr.getHostAddress()));
      setAddress(addr);
    }
    catch (UnknownHostException e)
    {
      Log.d(TAG, String.format(Locale.ENGLISH, "Unable to resolve hostname '%s'", mHostName));
    }
  }
  
  public synchronized void setAddress(InetAddress inetAddr)
  {
    mInetAddress = inetAddr;
  }
  
  public synchronized InetAddress getAddress()
  {
    return mInetAddress;
  }
}

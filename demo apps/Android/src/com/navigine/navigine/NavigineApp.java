package com.navigine.navigine;
import com.navigine.navigine.*;
import com.navigine.naviginesdk.*;

import android.app.*;
import android.content.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.hardware.*;
import android.os.*;
import android.util.*;
import java.io.*;
import java.lang.*;
import java.nio.*;
import java.util.*;

public class NavigineApp extends Application
{
  public static NavigationThread Navigation = null;
  public static ReceiverThread Receiver = null;
  public static SenderThread Sender = null;
  public static IMUThread IMU = null;
  
  @Override public void onCreate()
  {
    super.onCreate();
  }
  
  public static void setTrackFile()
  {
    if (NavigineApp.Navigation == null)
      return;
    
    String archivePath = NavigineApp.Navigation.getArchivePath();
    if (archivePath != null && archivePath != null)
    {
      for(int i = 1; true; ++i)
      {
        String suffix = String.format(Locale.ENGLISH, ".%d.txt", i);
        String trackFile = archivePath.replaceAll("\\.zip$", suffix);
        if ((new File(trackFile)).exists())
          continue;
        NavigineApp.Navigation.setTrackFile(trackFile);
        break;
      }
    }
  }
}

package com.navigine.navigine;
import com.navigine.navigine.*;
import com.navigine.naviginesdk.*;

import android.app.*;
import android.content.*;
import android.database.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.hardware.*;
import android.net.Uri;
import android.os.*;
import android.provider.MediaStore;
import android.text.*;
import android.text.method.*;
import android.view.*;
import android.view.View.*;
import android.widget.*;
import android.widget.ImageView.*;
import android.util.*;
import java.io.*;
import java.lang.*;
import java.nio.*;
import java.net.*;
import java.util.*;

public class MainActivity extends Activity
{
  // Constants
  private static final String TAG = "Navigine.MainActivity";
  
  public static final String  DEFAULT_SERVER    = "server.navigine.com";
  public static final int     DEFAULT_SEND_PORT = 27015;
  public static final int     DEFAULT_RECV_PORT = 27016;
  public static final int     DEFAULT_FREQUENCY = 10;
  
  // This context
  private final Context context = this;
  
  // GUI parameters
  private Button mLocationManagementButton = null;
  private Button mNavigationModeButton = null;
  private Button mMeasuringModeButton  = null;
  private Button mTextModeButton       = null;
  
  private boolean mBeaconServiceStarted = false;
  
  /** Called when the activity is first created */
  @Override public void onCreate(Bundle savedInstanceState)
  {
    Log.d(TAG, "MainActivity created");
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    setContentView(R.layout.main);
    
    // Setting up GUI parameters
    mLocationManagementButton = (Button)findViewById(R.id.location_management_button);
    mNavigationModeButton = (Button)findViewById(R.id.navigation_mode_button);
    mMeasuringModeButton  = (Button)findViewById(R.id.measuring_mode_button);
    mTextModeButton       = (Button)findViewById(R.id.text_mode_button);
    
    mLocationManagementButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          // Starting Loader activity
          Log.d(TAG, "Loading location management mode");
          Intent I = new Intent(context, LoaderActivity.class);
          startActivity(I);
        }
      });
    
    mNavigationModeButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          // Starting Navigation activity
          Log.d(TAG, "Loading navigation mode");
          Intent I = new Intent(context, NavigationActivity.class);
          startActivity(I);
        }
      });
    
    mMeasuringModeButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          // Starting Measuring activity
          Log.d(TAG, "Loading measuring mode");
          Intent I = new Intent(context, MeasuringActivity.class);
          startActivity(I);
        }
      });
    
    mTextModeButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          // Starting Text activity
          Log.d(TAG, "Loading text mode");
          Intent I = new Intent(context, TextActivity.class);
          startActivity(I);
        }
      });
    
    // Setting static parameters
    BeaconService.DEBUG_LEVEL     = 2;
    NativeUtils.DEBUG_LEVEL       = 2;
    LocationLoader.DEBUG_LEVEL    = 2;
    NavigationThread.DEBUG_LEVEL  = 3;
    MeasurerThread.DEBUG_LEVEL    = 3;
    SensorThread.DEBUG_LEVEL      = 2;
    Parser.DEBUG_LEVEL            = 2;
    
    NavigationThread.STRICT_MODE  = false;
    NavigationThread.HEAT_MAP_ENABLED = true;
    LocationLoader.SERVER = "client.navigine.com";
    
    try
    {
      NavigineApp.Navigation = new NavigationThread(getApplicationContext());
    }
    catch (Throwable e)
    {
      NavigineApp.Navigation = null;
      Log.e(TAG, Log.getStackTraceString(e));
      return;
    }
    
    NavigineApp.Receiver = new ReceiverThread();
    NavigineApp.Sender = new SenderThread();
    NavigineApp.IMU = new IMUThread();
    
    SharedPreferences settings = getApplicationContext().getSharedPreferences("MainSettings", 0);
    if (settings.getString("map_file", "").length() == 0 ||
        !NavigineApp.Navigation.loadArchive(settings.getString("map_file", "")))
    {
      mNavigationModeButton.setVisibility(LinearLayout.GONE);
      mMeasuringModeButton.setVisibility(LinearLayout.GONE);
      SharedPreferences.Editor editor = settings.edit();
      editor.remove("map_file");
      editor.commit();
    }
    
    if (settings.getBoolean("log_input_mode", false))
    {
      String logFile = settings.getString("log_input_file", "");
      if (logFile.length() > 0)
      {
        Log.d(TAG, "Loading input log file " + logFile);
        NavigineApp.Navigation.setInputFile(logFile);
      }
    }
    
    NavigineApp.Navigation.setScanTimeouts(
        settings.getInt("bluetooth_scan_timeout",
        MeasurerThread.BLUETOOTH_SCAN_TIMEOUT),
        MeasurerThread.BLUETOOTH_SLEEP_TIMEOUT);
    
    NavigineApp.Navigation.setFrequency(settings.getInt("navigation_frequency", DEFAULT_FREQUENCY));
    
    BeaconService.setUserId(settings.getString("user_hash", ""));
    BeaconService.setMapId(NavigineApp.Navigation.getLocation() == null ? 0 : NavigineApp.Navigation.getLocation().id);
    
    // Starting BeaconService (if necessary)
    if (settings.getBoolean("beacon_service_enabled", true))
    {
      if (!BeaconService.isStarted())
      {
        Log.d(TAG, "Starting BeaconService");
        startService(new Intent(context, BeaconService.class));
        mBeaconServiceStarted = true;
      }
    }
    
    Log.d(TAG, String.format(Locale.ENGLISH, "Location directory: %s",
          LocationLoader.getLocationDir(context, "")));
  }
  
  @Override public void onDestroy()
  {
    if (mBeaconServiceStarted)
      BeaconService.requestToStop();
    
    if (NavigineApp.Navigation != null)
    {
      Log.d(TAG, "Terminating Sender, Receiver, Navigation threads!");
      NavigineApp.IMU.terminate();
      NavigineApp.Sender.terminate();
      NavigineApp.Receiver.terminate();
      NavigineApp.Navigation.terminate();
      try
      {
        Log.d(TAG, "NavigationActivity: joining Sender thread");
        NavigineApp.Sender.join(1000);
        Log.d(TAG, "NavigationActivity: joining Receiver thread");
        NavigineApp.Receiver.join(1000);
        Log.d(TAG, "NavigationActivity: joining Navigation thread");
        NavigineApp.Navigation.join(1000);
        Log.d(TAG, "NavigationActivity: joining IMU thread");
        NavigineApp.IMU.join(1000);
      }
      catch (Throwable e)
      {
        Log.e(TAG, "Joining error!");
        Log.e(TAG, Log.getStackTraceString(e));
      }
    }
    Log.d(TAG, "MainActivity destroyed");
    super.onDestroy();
  }
  
  @Override public void onStart()
  {
    Log.d(TAG, "MainActivity started");
    super.onStart();
    
    if (NavigineApp.Navigation == null)
    {
      Toast.makeText(getApplicationContext(), "Unable to create Navigation thread!", Toast.LENGTH_LONG).show();
      mLocationManagementButton.setVisibility(LinearLayout.GONE);
      mNavigationModeButton.setVisibility(LinearLayout.GONE);
      mMeasuringModeButton.setVisibility(LinearLayout.GONE);
      mTextModeButton.setVisibility(LinearLayout.GONE);
      return;
    }
    
    SharedPreferences settings = getApplicationContext().getSharedPreferences("MainSettings", 0);
    boolean hasMap = settings.getString("map_file", "").length() > 0;
    mNavigationModeButton.setVisibility(hasMap ? LinearLayout.VISIBLE : LinearLayout.GONE);
    mMeasuringModeButton.setVisibility(hasMap ? LinearLayout.VISIBLE : LinearLayout.GONE);
  }
}

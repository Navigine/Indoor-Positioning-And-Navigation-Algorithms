package com.navigine.navigine;
import com.navigine.navigine.*;
import com.navigine.naviginesdk.*;

import android.app.*;
import android.content.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.hardware.*;
import android.location.*;
import android.net.wifi.*;
import android.os.*;
import android.text.method.*;
import android.view.*;
import android.view.View.*;
import android.widget.*;
import android.util.*;
import java.io.*;
import java.lang.*;
import java.nio.*;
import java.net.*;
import java.util.*;

public class TextActivity extends Activity
{
  // Constants
  private static final String   TAG                 = "Navigine.TextActivity";
  private static final int      REQUEST_PICK_FILE   = 1;  
  private static final int      REQUEST_PICK_MAP    = 2;  
  
  // This object
  private Context mContext = this;
  private Context mApplicationContext = null;
  
  // UI parameters
  private CheckBox    mServerModeCheckBox = null;
  private CheckBox    mLogOutputCheckBox  = null;
  private CheckBox    mLogInputCheckBox   = null;
  private Button      mStartButton        = null;
  private EditText    mAddressEdit        = null;
  private TextView    mTextView           = null;
  private TextView    mTextView2          = null;
  private TimerTask   mTimerTask          = null;
  private Handler     mHandler            = new Handler();
  private Timer       mTimer              = new Timer();
  
  private String      mLogInputFile       = "";
  private String      mLogOutputFile      = "";
  
  private boolean     mBackgroundActiveMode = true;
  
  // State parameters
  private String mHost = MainActivity.DEFAULT_SERVER;
  
  /** Called when the activity is first created. */
  @Override public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    setContentView(R.layout.text_mode);
    
    mApplicationContext = getApplicationContext();
    
    // Initializing variables
    mServerModeCheckBox = (CheckBox)findViewById(R.id.server_mode_checkbox);
    mLogOutputCheckBox = (CheckBox)findViewById(R.id.log_output_checkbox);
    mLogInputCheckBox = (CheckBox)findViewById(R.id.log_mode_checkbox);
    mStartButton = (Button)findViewById(R.id.start_button);
    mAddressEdit = (EditText)findViewById(R.id.address_edit);
    mTextView2 = (TextView)findViewById(R.id.text_view_2);
    mTextView  = (TextView)findViewById(R.id.text_view);
    
    mTextView.setMovementMethod(new ScrollingMovementMethod());
    
    // Restoring settings
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    mHost = settings.getString("navigation_server_address", MainActivity.DEFAULT_SERVER);
    mLogInputFile = settings.getString("log_input_file", "");
    mBackgroundActiveMode = settings.getBoolean("background_active_mode", true);
    mServerModeCheckBox.setChecked(settings.getBoolean("navigation_server_enabled", false));
    mLogInputCheckBox.setChecked(settings.getBoolean("log_input_mode", false));
    
    mAddressEdit.setText(mHost);
    
    NavigineApp.Navigation.idle();
    NavigineApp.Navigation.setScanEnabled(true);
    
    // Setting up CONNECT button click handler
    mStartButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          if (NavigineApp.Sender.getConnectionState() == SenderThread.STATE_IDLE &&
              NavigineApp.Receiver.getConnectionState() == ReceiverThread.STATE_IDLE &&
              NavigineApp.Navigation.getCurrentState() == NavigationThread.STATE_IDLE)
            connect();
          else
            disconnect();
        }
      });
    
    mServerModeCheckBox.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          disconnect();
        }
      });
    
    mLogInputCheckBox.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          disconnect();
          
          SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
          SharedPreferences.Editor editor = settings.edit();
          editor.putBoolean("log_input_mode", mLogInputCheckBox.isChecked());
          editor.commit();
          
          if (mLogInputCheckBox.isChecked())
          {
            Intent intent = new Intent(mContext, FilePickerActivity.class);
            startActivityForResult(intent, REQUEST_PICK_FILE);
          }
          else
            cancelInputFile();
        }
      });
    
    mLogOutputCheckBox.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          disconnect();
          
          String filename = "";
          if (mLogOutputCheckBox.isChecked())
            filename = getOutputFile();
          NavigineApp.Navigation.setOutputFile(filename);
        }
      });
  }
  
  @Override public void onDestroy()
  {
    super.onDestroy();
    
    NavigineApp.Navigation.idle();
  }
  
  @Override public void onStart()
  {
    super.onStart();
    
    // Starting interface updates
    mTimerTask = 
      new TimerTask()
      {
        @Override public void run() 
        {
          update();
        }
      };
    mTimer.schedule(mTimerTask, 500, 100);
  }
  
  @Override public void onStop()
  {
    super.onStop();
    
    mTimerTask.cancel();
    mTimerTask = null;
  }
  
  @Override public boolean onCreateOptionsMenu(Menu menu)
  {
    MenuInflater inflater = getMenuInflater();
    inflater.inflate(R.menu.text_menu, menu);
    menu.findItem(R.id.text_menu_disable_beacon_service).setVisible(BeaconService.isStarted());
    menu.findItem(R.id.text_menu_enable_beacon_service).setVisible(!BeaconService.isStarted());
    return true;
  }
  
  @Override public boolean onPrepareOptionsMenu(Menu menu)
  {
    menu.findItem(R.id.text_menu_disable_beacon_service).setVisible(BeaconService.isStarted());
    menu.findItem(R.id.text_menu_enable_beacon_service).setVisible(!BeaconService.isStarted());
    menu.findItem(R.id.text_menu_background_active_mode).setVisible(!mBackgroundActiveMode);
    menu.findItem(R.id.text_menu_background_passive_mode).setVisible(mBackgroundActiveMode);
    return true;
  }
  
  @Override public boolean onOptionsItemSelected(MenuItem item)
  {
    switch (item.getItemId())
    {
      case R.id.text_menu_load_map:
        Intent intent = new Intent(mContext, FilePickerActivity.class);
        startActivityForResult(intent, REQUEST_PICK_MAP);
        return true;
      
      case R.id.text_menu_set_navigation_frequency:
        showParameterDialog("navigation_frequency", "Navigation frequency, Hz", 1, 100, MainActivity.DEFAULT_FREQUENCY);
        return true;
      
      case R.id.text_menu_set_bluetooth_scan_timeout:
        showParameterDialog("bluetooth_scan_timeout", "Bluetooth scan timeout, ms", 0, 9999, 1000);
        return true;
      
      case R.id.text_menu_disable_beacon_service:
      {
        Log.d(TAG, "Terminating beacon service");
        BeaconService.requestToStop();
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean("beacon_service_enabled", false);
        editor.commit();
        return true;
      }
      
      case R.id.text_menu_enable_beacon_service:
      {
        if (!BeaconService.isStarted())
        {
          Log.d(TAG, "Starting BeaconService");
          startService(new Intent(mApplicationContext, BeaconService.class));
        }
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean("beacon_service_enabled", true);
        editor.commit();
        return true;
      }
      
      case R.id.text_menu_background_active_mode:
      {
        mBackgroundActiveMode = true;
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean("background_active_mode", true);
        editor.commit();
        return true;
      }
        
      case R.id.text_menu_background_passive_mode:
      {
        mBackgroundActiveMode = false;
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean("background_active_mode", false);
        editor.commit();
        return true;        
      }
      
      default:
        return super.onOptionsItemSelected(item);
    }
  }
  
  @Override protected void onActivityResult(int requestCode, int resultCode, Intent data)
  {
    if (resultCode != RESULT_OK)
      return;
    
    switch (requestCode)
    {
      case REQUEST_PICK_FILE:
        if (data.hasExtra(FilePickerActivity.EXTRA_FILE_PATH))
        {
          // Get the file path
          File f = new File(data.getStringExtra(FilePickerActivity.EXTRA_FILE_PATH));
          setInputFile(f.getPath());
        }
        break;
      
      case REQUEST_PICK_MAP:
        if (data.hasExtra(FilePickerActivity.EXTRA_FILE_PATH))
        {
          // Get the file path
          File f = new File(data.getStringExtra(FilePickerActivity.EXTRA_FILE_PATH));
          
          // Loading data
          if (!loadMap(f.getPath()))
          {
            AlertDialog.Builder alertBuilder = new AlertDialog.Builder(mContext);
            alertBuilder.setTitle(getString(R.string.error));
            alertBuilder.setMessage(getString(R.string.unable_to_load_map));
            alertBuilder.setNegativeButton(getString(R.string.ok_button),
              new DialogInterface.OnClickListener()
              {
                @Override public void onClick(DialogInterface dlg, int id)
                {
                  dlg.cancel();
                }
              });
            AlertDialog alertDialog = alertBuilder.create();
            alertDialog.setCanceledOnTouchOutside(false);
            alertDialog.show();
            return;
          }
          
          SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
          SharedPreferences.Editor editor = settings.edit();
          editor.putString("map_file", f.getPath());
          editor.commit();
        }
        
      default:
        break;
    }
  }

  private boolean loadMap(String filename)
  {
    if (!NavigineApp.Navigation.loadArchive(filename))
    {
      String error = NavigineApp.Navigation.getLastError();
      if (error != null)
        Toast.makeText(mContext, (CharSequence)error, Toast.LENGTH_LONG).show();
      SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
      SharedPreferences.Editor editor = settings.edit();
      editor.remove("map_file");
      editor.commit();
      return false;
    }
    return true;
  }
  
  TextView _textView = null;
  EditText _editText = null;
  String _paramName  = null;
  int _minValue = 0;
  int _maxValue = 0;
  
  void showParameterDialog(String paramName, String paramDesc, int minValue, int maxValue, int defaultValue)
  {
    _paramName = paramName;
    _minValue  = minValue;
    _maxValue  = maxValue;
    
    LayoutInflater inflater = getLayoutInflater();
    View view = inflater.inflate(R.layout.set_parameter_dialog, null);
    String title = String.format(Locale.ENGLISH, "Set %s", paramDesc);
    _textView = (TextView)view.findViewById(R.id.set_parameter_name);
    _editText = (EditText)view.findViewById(R.id.set_parameter_edit);
    _editText.setTypeface(Typeface.MONOSPACE);
    
    SharedPreferences settings = getApplicationContext().getSharedPreferences("MainSettings", 0);
    _editText.setText(String.format(Locale.ENGLISH, "%d", settings.getInt(paramName, defaultValue)));
    
    _textView.setText(String.format(Locale.ENGLISH, "Value (%d-%d):", minValue, maxValue));
    
    AlertDialog.Builder alertBuilder = new AlertDialog.Builder(mContext);
    alertBuilder.setView(view);
    alertBuilder.setTitle(title);
    
    alertBuilder.setPositiveButton(getString(R.string.ok_button),
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        {
          try
          {
            int value = Integer.parseInt(_editText.getText().toString());
            value = Math.max(value, _minValue);
            value = Math.min(value, _maxValue);
            SharedPreferences settings = getApplicationContext().getSharedPreferences("MainSettings", 0);
            SharedPreferences.Editor editor = settings.edit();
            editor.putInt(_paramName, value);
            editor.commit();
            if (_paramName.equals("bluetooth_scan_timeout"))
              NavigineApp.Navigation.setScanTimeouts(value, 100);
            if (_paramName.equals("navigation_frequency"))
              NavigineApp.Navigation.setFrequency(value);
          }
          catch (Throwable e)
          {
          }
          dlg.cancel();
        }
      });
    
    alertBuilder.setNegativeButton(getString(R.string.cancel_button),
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        {
          dlg.cancel();
        }
      });
    
    AlertDialog alertDialog = alertBuilder.create();
    alertDialog.setCanceledOnTouchOutside(false);
    alertDialog.show();
  }
  
  private synchronized void connect()
  {
    // Reading server IP-address from input form
    mHost = mAddressEdit.getText().toString();
    if (mHost.length() == 0)
      mHost = MainActivity.DEFAULT_SERVER;
    
    Log.d(TAG, String.format(Locale.ENGLISH, "Reconnecting to %s...", mHost));
    
    String outLogFile = "";
    if (mLogOutputCheckBox.isChecked())
      outLogFile = getOutputFile();
    NavigineApp.Navigation.setOutputFile(outLogFile);
    
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    SharedPreferences.Editor editor = settings.edit();
    editor.putString("navigation_server_address", mHost);
    editor.putBoolean("navigation_server_enabled", mServerModeCheckBox.isChecked());
    editor.putBoolean("log_input_mode", mLogInputCheckBox.isChecked());
    editor.putString("log_input_file", mLogInputFile);
    editor.commit();
    
    if (mServerModeCheckBox.isChecked())
    {
      // Server mode navigation
      int frequency = settings.getInt("navigation_frequency", MainActivity.DEFAULT_FREQUENCY);
      NavigineApp.Sender.reconnect(mHost, MainActivity.DEFAULT_SEND_PORT, frequency);
      NavigineApp.Receiver.idle();//reconnect(mHost, MainActivity.DEFAULT_RECV_PORT);
      NavigineApp.Navigation.work();
      NavigineApp.setTrackFile();
    }
    else
    {
      // Device mode navigation
      NavigineApp.Sender.idle();
      NavigineApp.Receiver.idle();
      NavigineApp.Navigation.work();
      NavigineApp.setTrackFile();
    }
  }
  
  private synchronized void disconnect()
  {
    NavigineApp.Sender.idle();
    NavigineApp.Receiver.idle();
    NavigineApp.Navigation.idle();
    NavigineApp.Navigation.setScanEnabled(true);
  }
  
  private void update()
  {
    mHandler.post(mRunnable);
  }
  
  private void setInputFile(String filename)
  {
    mLogInputFile = new String(filename);
    Log.d(TAG, "Loading input log file " + mLogInputFile);
    NavigineApp.Navigation.setInputFile(mLogInputFile);
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    SharedPreferences.Editor editor = settings.edit();
    editor.putBoolean("log_input_mode", true);
    editor.putString("log_input_file", mLogInputFile);
    editor.commit();
  }
  
  private void cancelInputFile()
  {
    mLogInputFile = "";
    Log.d(TAG, "Cancel input log file");
    NavigineApp.Navigation.cancelInputFile();
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    SharedPreferences.Editor editor = settings.edit();
    editor.putBoolean("log_input_mode", false);
    editor.putString("log_input_file", mLogInputFile);
    editor.commit();
  }
  
  private String getOutputFile()
  {
    String arhivePath = NavigineApp.Navigation.getArchivePath();
    if (arhivePath != null && arhivePath.length() > 0)
    {
      for(int i = 1; true; ++i)
      {
        String suffix = String.format(Locale.ENGLISH, ".%d.log", i);
        String filename = arhivePath.replaceAll("\\.zip$", suffix);
        if (!(new File(filename)).exists())
          return filename;
      }
    }
    return "";
  }
  
  final Runnable mRunnable =
    new Runnable()
    {
      public void run()
      {
        try
        {
          long timeNow = DateTimeUtils.currentTimeMillis();
          boolean serverMode = mServerModeCheckBox.isChecked();
          StringBuilder messageBuilder = new StringBuilder();
          
          // Determine button state
          if (NavigineApp.Sender.getConnectionState() == SenderThread.STATE_IDLE &&
              NavigineApp.Receiver.getConnectionState() == ReceiverThread.STATE_IDLE &&
              NavigineApp.Navigation.getCurrentState() == NavigationThread.STATE_IDLE)
            mStartButton.setText(getString(R.string.start_button));
          else
            mStartButton.setText(getString(R.string.stop_button));
          
          // Preparing display message
          
          String archivePath = NavigineApp.Navigation.getArchivePath();
          if (archivePath != null && archivePath.length() > 0)
          {
            String name = new File(archivePath).getName();
            messageBuilder.append(String.format(Locale.ENGLISH, "Archive: '%s'\n", name));
          }
          
          String inputFile = NavigineApp.Navigation.getInputFile();
          if (inputFile != null &&  inputFile.length() > 0)
          {
            String name = new File(inputFile).getName();
            messageBuilder.append(String.format(Locale.ENGLISH, "Input file: '%s'\n", name));
          }
          
          String outputFile = NavigineApp.Navigation.getOutputFile();
          if (outputFile != null &&  outputFile.length() > 0)
          {
            String name = new File(outputFile).getName();
            messageBuilder.append(String.format(Locale.ENGLISH, "Log: '%s'\n", name));
          }
          
          if (serverMode)
          {
            mAddressEdit.setVisibility(View.VISIBLE);
            
            switch (NavigineApp.Sender.getConnectionState())
            {
              case SenderThread.STATE_IDLE:
                messageBuilder.append(String.format(Locale.ENGLISH,
                                                    getString(R.string.disconnected_from_s_d),
                                                    mHost, MainActivity.DEFAULT_SEND_PORT));
                messageBuilder.append("\n");
                break;
              
              case SenderThread.STATE_CONNECT:
              case SenderThread.STATE_RECONNECT:
              case SenderThread.STATE_RESOLVE:
                messageBuilder.append(String.format(Locale.ENGLISH,
                                                    getString(R.string.connecting_to_s_d),
                                                    mHost, MainActivity.DEFAULT_SEND_PORT));
                messageBuilder.append("\n");
                break;
              
              case SenderThread.STATE_NORMAL:
                messageBuilder.append(String.format(Locale.ENGLISH,
                                                    getString(R.string.connected_to_s_d),
                                                    mHost, MainActivity.DEFAULT_SEND_PORT));
                messageBuilder.append("\n");
                break;
              
              default:
                messageBuilder.append("\n");
            }
            
            switch (NavigineApp.Receiver.getConnectionState())
            {
              case ReceiverThread.STATE_IDLE:
                messageBuilder.append(String.format(Locale.ENGLISH,
                                                    getString(R.string.disconnected_from_s_d),
                                                    mHost, MainActivity.DEFAULT_RECV_PORT));
                messageBuilder.append("\n");
                break;
              
              case ReceiverThread.STATE_CONNECT:
              case ReceiverThread.STATE_RECONNECT:
              case ReceiverThread.STATE_RESOLVE:
                messageBuilder.append(String.format(Locale.ENGLISH,
                                                    getString(R.string.connecting_to_s_d),
                                                    mHost, MainActivity.DEFAULT_RECV_PORT));
                messageBuilder.append("\n");
                break;
                
              case ReceiverThread.STATE_NORMAL:
                messageBuilder.append(String.format(Locale.ENGLISH,
                                                    getString(R.string.connected_to_s_d),
                                                    mHost, MainActivity.DEFAULT_RECV_PORT));
                messageBuilder.append("\n");
                break;
              
              default:
                messageBuilder.append("\n");
            }
            
            if (NavigineApp.Sender.getConnectionState() == SenderThread.STATE_NORMAL)
            {
              int nsecs = NavigineApp.Sender.getTotalTime();
              int kbyt  = NavigineApp.Sender.getTotalBytes() / 1024;
              int kbps  = nsecs == 0 ? 0 : kbyt / nsecs;
              messageBuilder.append(String.format(Locale.ENGLISH, "Total bytes send: %d Kb in %d seconds (%d Kb/s)\n" +
                                                  "Messages: total=%d  send=%d  lost=%d  buf=%d\n", kbyt, nsecs, kbps,
                                                  NavigineApp.Sender.getTotalMessages(),
                                                  NavigineApp.Sender.getSendMessages(),
                                                  NavigineApp.Sender.getLostMessages(),
                                                  NavigineApp.Sender.getBufMessages()));
            }
          }
          else
          {
            mAddressEdit.setVisibility(View.GONE);
            
            if (NavigineApp.Navigation.getCurrentState() == NavigationThread.STATE_NORMAL)
            {
              messageBuilder.append(String.format(Locale.ENGLISH,"Messages: total=%d  proc=%d  lost=%d  buf=%d\n",
                                                  NavigineApp.Navigation.getTotalMessages(),
                                                  NavigineApp.Navigation.getProcMessages(),
                                                  NavigineApp.Navigation.getLostMessages(),
                                                  NavigineApp.Navigation.getBufMessages()));
            }
          }
          
          int errorCode = NavigineApp.Navigation.getErrorCode();
          if (errorCode > 0)
            messageBuilder.append(String.format(Locale.ENGLISH, "ErrorCode: %d\n", errorCode));
          
          mTextView2.setText(messageBuilder.toString());
          messageBuilder = new StringBuilder();
          //messageBuilder.append("\n");
          
          // Writing WiFi scan results
          List<WScanResult> scanResults = NavigineApp.Navigation.getScanResults(0);
          
          List<WScanResult> wifiEntries   = new ArrayList<WScanResult>();
          List<WScanResult> bleEntries    = new ArrayList<WScanResult>();
          List<WScanResult> beaconEntries = new ArrayList<WScanResult>();
          Set<String> bssids = new TreeSet<String>();
          int wifiEntriesCounter = 0;
          int bleEntriesCounter  = 0;
          int beaconEntriesCounter = 0;
          
          for(int i = scanResults.size() - 1; i >= 0; --i)
          {
            WScanResult result = scanResults.get(i);
            if (result.time > timeNow)
              continue;
            
            switch (result.type)
            {
              case WScanResult.TYPE_WIFI:
                wifiEntriesCounter++;
                if (!bssids.contains(result.BSSID))
                  wifiEntries.add(result);
                break;
              
              case WScanResult.TYPE_BLE:
                bleEntriesCounter++;
                if (!bssids.contains(result.BSSID))
                  bleEntries.add(result);
                break;
              
              case WScanResult.TYPE_BEACON:
                beaconEntriesCounter++;
                if (!bssids.contains(result.BSSID))
                  beaconEntries.add(result);
                break;
            }
            
            bssids.add(result.BSSID);
          }
          
          Collections.sort(wifiEntries, new Comparator<WScanResult>() {
            @Override public int compare(WScanResult result1, WScanResult result2) {
              return result1.level > result2.level ? -1 : result1.level < result2.level ? 1 : 0;
          }});
          
          Collections.sort(bleEntries, new Comparator<WScanResult>() {
            @Override public int compare(WScanResult result1, WScanResult result2) {
              return result1.level > result2.level ? -1 : result1.level < result2.level ? 1 : 0;
            }});
          
          Collections.sort(beaconEntries, new Comparator<WScanResult>() {
            @Override public int compare(WScanResult result1, WScanResult result2) {
              return result1.distance < result2.distance ? -1 : result1.distance > result2.distance ? 1 : 0;
            }});
          
          messageBuilder.append(String.format(Locale.ENGLISH, "Wi-Fi networks (%d), entries/sec: %.1f\n",
                                wifiEntries.size(), (float)wifiEntriesCounter * 1000.0f / MeasurerThread.SCAN_MAX_DELAY));
          
          for(int i = 0; i < wifiEntries.size(); ++i)
          {
            if (i >= 5)
            {
              messageBuilder.append("...\n");
              break;
            }
            WScanResult result = wifiEntries.get(i);
            String name = result.SSID.length() <= 11 ? result.SSID : result.SSID.substring(0, 10) + "...";
            messageBuilder.append(String.format(Locale.ENGLISH, "%s \t  %.1f\t  (%.1f sec)\t  (%s)\n",
                                                result.BSSID, (float)result.level,
                                                (float)(timeNow - result.time) / 1000,
                                                name));
          }
          messageBuilder.append("\n");
          
          messageBuilder.append(String.format(Locale.ENGLISH, "BLE devices (%d), entries/sec: %.1f\n",
                                bleEntries.size(), (float)bleEntriesCounter * 1000.0f / MeasurerThread.SCAN_MAX_DELAY));
          
          for(int i = 0; i < bleEntries.size(); ++i)
          {
            if (i >= 5)
            {
              messageBuilder.append("...\n");
              break;
            }
            WScanResult result = bleEntries.get(i);
            messageBuilder.append(String.format(Locale.ENGLISH, "%s \t  %.1f\t  (%.1f sec)\n",
                                                result.BSSID, (float)result.level,
                                                (float)(timeNow - result.time) / 1000));
          }
          messageBuilder.append("\n");
          
          messageBuilder.append(String.format(Locale.ENGLISH, "BEACONs (%d), entries/sec: %.1f\n",
                                beaconEntries.size(), (float)beaconEntriesCounter * 1000.0f / MeasurerThread.SCAN_MAX_DELAY));
          
          for(int i = 0; i < beaconEntries.size(); ++i)
          {
            if (i >= 5)
            {
              messageBuilder.append("...\n");
              break;
            }
            WScanResult result = beaconEntries.get(i);
            String address = result.BSSID.substring(0, 18) + "...)";
            messageBuilder.append(String.format(Locale.ENGLISH, "%s \t%.1f \t%.1fm \t(%.1f sec)   BAT=%d%%\n",
                                                address, (float)result.level, result.distance,
                                                (float)(timeNow - result.time) / 1000,
                                                result.battery));
          }
          messageBuilder.append("\n");
          
          // Writing accelerometer values
          float[] accelVector = NavigineApp.Navigation.getAccelVector();
          if (accelVector != null)
            messageBuilder.append(String.format(Locale.ENGLISH, "Accelerometer: (%.4f, %.4f, %.4f)\n",
                                                accelVector[0], accelVector[1], accelVector[2]));
          
          // Writing magnetometer values
          float[] magnetVector = NavigineApp.Navigation.getMagnetVector();
          if (magnetVector != null)
            messageBuilder.append(String.format(Locale.ENGLISH, "Magnetometer: (%.4f, %.4f, %.4f)\n",
                                                magnetVector[0], magnetVector[1], magnetVector[2]));
          
          // Writing gyroscope values
          float[] gyroVector = NavigineApp.Navigation.getGyroVector();
          if (gyroVector != null)
            messageBuilder.append(String.format(Locale.ENGLISH, "Gyroscope: (%.4f, %.4f, %.4f)\n",
                                                gyroVector[0], gyroVector[1], gyroVector[2]));
          
          // Writing orientation values
          float[] orientVector = NavigineApp.Navigation.getOrientVector();
          if (orientVector != null)
            messageBuilder.append(String.format(Locale.ENGLISH, "Orientation: (%.4f, %.4f, %.4f)\n",
                                                orientVector[0], orientVector[1], orientVector[2]));
          
          // Writing location (gps) coordinates
          double[] locVector = NavigineApp.Navigation.getLocationVector();
          if (locVector != null)
            messageBuilder.append(String.format(Locale.ENGLISH, "GPS: (%.8f, %.8f, %.2f, %.2f)\n\n",
                                                locVector[0], locVector[1], locVector[2], locVector[3]));
          
          List<DeviceInfo> devices = NavigineApp.Navigation.getDeviceList();
          for(int i = 0; i < devices.size(); ++i)
          {
            DeviceInfo info = devices.get(i);
            messageBuilder.append(String.format(Locale.ENGLISH, "Device %d: %s: (%.2f, %.2f)\n",
                                  info.index, info.id, info.x, info.y));
          }
          mTextView.setText(messageBuilder.toString());
        }
        catch (Throwable e)
        {
          Log.e(TAG, Log.getStackTraceString(e));
        }
      }
    };
}

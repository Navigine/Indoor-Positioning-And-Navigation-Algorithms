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

public class LoaderActivity extends Activity
{
  // Constants
  public static final String TAG = "Navigine.LoaderActivity";
  public static final int LOADER_TIMEOUT = 30000;
  
  // This context
  private Context mContext = this;
  private Context mApplicationContext = this;
  
  private ListView    mListView     = null;
  private TextView    mStatusLabel  = null;
  private TimerTask   mTimerTask    = null;
  private Handler     mHandler      = new Handler();
  private Timer       mTimer        = new Timer();
  
  private int mLoader = -1;
  private long mLoaderTime = -1;
  private List<LocationInfo> mInfoList = new ArrayList<LocationInfo>();
  
  public static final int DOWNLOAD = 1;
  public static final int UPLOAD   = 2;
  private class LoaderState
  {
    public String location = "";
    public int type = 0;
    public int id = -1;
    public int state = 0;
    public long timeLabel = 0;
  }
  private Map<String, LoaderState> mLoaderMap = new TreeMap<String, LoaderState>();
  
  private class LoaderAdapter extends BaseAdapter
  { 
    @Override public int getCount()
    {
      return mInfoList.size(); 
    }
    
    @Override public Object getItem(int pos)
    {
      return mInfoList.get(pos);
    }
    
    @Override public long getItemId(int pos)
    {
      return pos;
    }
    
    public void updateList()
    {
      notifyDataSetChanged();
    }
    
    @Override public View getView(final int position, View convertView, ViewGroup parent)
    {
      View view = convertView;
      if (view == null)
      {
        LayoutInflater inflater = (LayoutInflater)mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        view = inflater.inflate(R.layout.content_list_item, null);
      }
      TextView titleTextView = (TextView)view.findViewById(R.id.list_item_title);
      TextView stateTextView = (TextView)view.findViewById(R.id.list_item_state);
      TextView downTextView = (TextView)view.findViewById(R.id.list_item_downbar);
      Button downloadButton = (Button)view.findViewById(R.id.list_item_download_button);
      Button uploadButton = (Button)view.findViewById(R.id.list_item_upload_button);
      
      LocationInfo info = mInfoList.get(position);
      String titleText = info.title;
      String stateText = "";
      
      if (titleText.length() > 30)
        titleText = titleText.substring(0, 28) + "...";
      
      synchronized (mLoaderMap)
      {
        if (mLoaderMap.containsKey(info.title))
        {
          LoaderState loader = mLoaderMap.get(info.title);
          if (loader.state < 100)
            stateText = String.format(Locale.ENGLISH, "%d%%", loader.state);
          else if (loader.state == 100)
            stateText = String.format(Locale.ENGLISH, "Done!");
          else
            stateText = String.format(Locale.ENGLISH, "Failed!");
        }
      }
      
      if (info.localVersion < 0)
        titleText += " (?)";
      else
      {
        if (info.localModified)
          titleText += String.format(Locale.ENGLISH, " (v. %d+)", info.localVersion);
        else
          titleText += String.format(Locale.ENGLISH, " (v. %d)", info.localVersion);
      }
      
      SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
      String mapFile = settings.getString("map_file", "");
      if (mapFile.equals(info.archiveFile))
      {
        titleTextView.setTypeface(null, Typeface.BOLD);
        view.setBackgroundColor(Color.parseColor("#590E0E"));
      }
      else
      {
        titleTextView.setTypeface(null, Typeface.NORMAL);
        view.setBackgroundColor(Color.BLACK);
      }
      
      titleTextView.setText((CharSequence)titleText);
      stateTextView.setText((CharSequence)stateText);
      
      if (info.localModified)
      {
        downloadButton.setVisibility(LinearLayout.GONE);
        uploadButton.setVisibility(LinearLayout.VISIBLE);
        downTextView.setText("Version is modified. Upload?");
      }
      else if (info.serverVersion > info.localVersion)
      {
        downloadButton.setVisibility(LinearLayout.VISIBLE);
        uploadButton.setVisibility(LinearLayout.GONE);
        String downText = String.format(Locale.ENGLISH, "Version available: %d", info.serverVersion);
        downTextView.setText((CharSequence)downText);
      }
      else
      {
        downloadButton.setVisibility(LinearLayout.INVISIBLE);
        uploadButton.setVisibility(LinearLayout.GONE);
        downTextView.setText("Version is up to date");
      }
      
      downloadButton.setOnClickListener(new View.OnClickListener()
        {
          @Override public void onClick(View v)
          {
            startDownload(position);
          }
        });
      
      uploadButton.setOnClickListener(new View.OnClickListener()
        {
          @Override public void onClick(View v)
          {
            startUpload(position);
          }
        });
      
      return view; 
    }
  }
  private LoaderAdapter mAdapter = null;
  
  /** Called when the activity is first created */
  @Override public void onCreate(Bundle savedInstanceState)
  {
    Log.d(TAG, "LoaderActivity created");
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    setContentView(R.layout.content);
    
    mApplicationContext = getApplicationContext();
    
    // Instantiate custom adapter
    mAdapter = new LoaderAdapter();
    
    // Handle listview and assign adapter
    mListView = (ListView)findViewById(R.id.content_list_view);
    mListView.setAdapter(mAdapter);
    mListView.setVisibility(LinearLayout.GONE);
    mListView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener()
      {
        public boolean onItemLongClick(AdapterView parent, View view, int position, long id)
        {
          selectItem(position);
          return true;
        }
      });
    
    mStatusLabel = (TextView)findViewById(R.id.content_status_label);
    mStatusLabel.setVisibility(LinearLayout.VISIBLE);
    
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    String userHash = settings.getString("user_hash", "");
    if (userHash.length() == 0)
      showUserHashDialog();
    else
      refreshMapList();
  }
  
  @Override public void onDestroy()
  {
    Log.d(TAG, "LoaderActivity destroyed");
    super.onDestroy();
  }
  
  @Override public void onStart()
  {
    Log.d(TAG, "LoaderActivity started");
    super.onStart();
    
    NavigineApp.Navigation.setScanEnabled(false);
    
    // Starting interface updates
    mTimerTask = 
      new TimerTask()
      {
        @Override public void run() 
        {
          mHandler.post(mRunnable);
        }
      };
    mTimer.schedule(mTimerTask, 100, 100);
  }
  
  @Override public void onStop()
  {
    Log.d(TAG, "LoaderActivity stopped");
    super.onStop();
    
    NavigineApp.Navigation.setScanEnabled(false);
    
    mTimerTask.cancel();
    mTimerTask = null;
  }
  
  @Override public boolean onCreateOptionsMenu(Menu menu)
  {
    Log.d(TAG, "Create menu options");
    MenuInflater inflater = getMenuInflater();
    inflater.inflate(R.menu.loader_menu, menu);
    menu.findItem(R.id.loader_menu_refresh_map_list).setVisible(mLoader < 0);   
    return true;
  }
  
  @Override public boolean onPrepareOptionsMenu(Menu menu)
  {
    Log.d(TAG, "Prepare menu options");
    menu.findItem(R.id.loader_menu_refresh_map_list).setVisible(mLoader < 0);   
    return true;
  }
  
  @Override public boolean onOptionsItemSelected(MenuItem item)
  {
    switch (item.getItemId())
    {
      case R.id.loader_menu_set_user_id:
      {
        showUserHashDialog();
        return true;
      }
      
      case R.id.loader_menu_refresh_map_list:
      {
        refreshMapList();
        return true;
      }
      
      default:
        return super.onOptionsItemSelected(item);
    }
  }
  
  private EditText _userEdit = null;
  
  private void showUserHashDialog()
  {
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    String userHash = settings.getString("user_hash", "");
    
    LayoutInflater inflater = getLayoutInflater();
    View view = inflater.inflate(R.layout.user_hash_dialog, null);
    _userEdit = (EditText)view.findViewById(R.id.user_hash_edit);
    _userEdit.setText((CharSequence)userHash);
    _userEdit.setTypeface(Typeface.MONOSPACE); 
    //_userEdit.addTextChangedListener(new TextWatcher()
    //  {
    //    public void afterTextChanged(Editable s) { }
    //    public void beforeTextChanged(CharSequence s, int start, int count, int after) { }
    //    public void onTextChanged(CharSequence s, int start, int before, int count)
    //    {
    //      String text = _userEdit.getText().toString();
    //      int length  = _userEdit.getText().length();
    //      
    //      if (text.endsWith("-"))
    //        return;
    //      
    //      if (count <= before)
    //        return;
    //      
    //      if (length == 4 || length == 9 || length == 14)
    //      {
    //        _userEdit.setText((CharSequence)(text + "-"));
    //        _userEdit.setSelection(length + 1);
    //      }
    //    }
    //  });
    
    AlertDialog.Builder alertBuilder = new AlertDialog.Builder(mContext);
    alertBuilder.setView(view);
    alertBuilder.setTitle("Enter user ID");
    alertBuilder.setNegativeButton(getString(R.string.cancel_button),
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        { }
      });
    
    alertBuilder.setPositiveButton(getString(R.string.ok_button),
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        {
          String userHash = _userEdit.getText().toString();
          SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
          SharedPreferences.Editor editor = settings.edit();
          editor.putString("user_hash", userHash);
          editor.commit();
          refreshMapList();
        }
      });
    
    AlertDialog dialog = alertBuilder.create();
    dialog.setCanceledOnTouchOutside(false);
    dialog.show();
  }
  
  LocationInfo _info = null;
  private void selectItem(int index)
  {
    _info = mInfoList.get(index);
    if (!(new File(_info.archiveFile)).exists())
    {
      String text = String.format(Locale.ENGLISH, "Location '%s' cannot be selected!\n" +
                                                  "Please, download location first!",
                                                  _info.title);
      Toast.makeText(mContext, (CharSequence)text, Toast.LENGTH_LONG).show();
      return;
    }
    
    AlertDialog.Builder alertBuilder = new AlertDialog.Builder(mContext);
    alertBuilder.setTitle(String.format(Locale.ENGLISH, "Location '%s'", _info.title));
    
    alertBuilder.setNegativeButton("Delete location",
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        {
          deleteLocation(_info);
        }
      });
    alertBuilder.setPositiveButton("Select location",
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        {
          selectLocation(_info);
        }
      });
    
    AlertDialog dialog = alertBuilder.create();
    dialog.setCanceledOnTouchOutside(false);
    dialog.show();
  }
  
  private void deleteLocation(LocationInfo info)
  {
    if (info != null)
    {
      try
      {
        (new File(info.archiveFile)).delete();
        info.localVersion = -1;
        info.localModified = false;
        
        String locationDir = LocationLoader.getLocationDir(mContext, info.title);
        File dir = new File(locationDir);
        File[] files = dir.listFiles();
        for(int i = 0; i < files.length; ++i)
          files[i].delete();
        dir.delete();
        
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        String mapFile = settings.getString("map_file", "");
        if (mapFile.equals(info.archiveFile))
        {
          NavigineApp.Navigation.loadArchive(null);
          SharedPreferences.Editor editor = settings.edit();
          editor.putString("map_file", "");
          editor.commit();
        }
        
        mAdapter.updateList();
      }
      catch (Throwable e)
      {
        Log.e(TAG, Log.getStackTraceString(e));
      }
    }
  }
  
  private void selectLocation(LocationInfo info)
  {
    if (info != null && NavigineApp.Navigation.loadArchive(info.archiveFile))
    {
      SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
      SharedPreferences.Editor editor = settings.edit();
      editor.putString("map_file", info.archiveFile);
      editor.commit();
      mAdapter.updateList();
    }
  }
  
  private void refreshMapList()
  {
    if (mLoader >= 0)
      return;
    
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    String userHash = settings.getString("user_hash", "");
    if (userHash.length() == 0)
      return;
    
    // Starting new loader
    String fileName = LocationLoader.getLocationDir(mApplicationContext, null) + "/maps.xml";
    new File(fileName).delete();
    mLoader = LocationLoader.startLocationLoader(userHash, null, fileName, true);
    mLoaderTime = DateTimeUtils.currentTimeMillis();
    mInfoList = new ArrayList<LocationInfo>();
    Log.d(TAG, String.format(Locale.ENGLISH, "Location loader started: %d", mLoader));
  }
  
  private void updateLocalVersions()
  {
    for(int i = 0; i < mInfoList.size(); ++i)
    {
      LocationInfo info = mInfoList.get(i);
      String versionStr = LocationLoader.getLocalVersion(mApplicationContext, info.title);
      if (versionStr != null)
      {
        Log.d(TAG, info.title + ": " + versionStr);
        info.localModified = versionStr.endsWith("+");
        if (info.localModified)
          versionStr = versionStr.substring(0, versionStr.length() - 1);
        try { info.localVersion = Integer.parseInt(versionStr); } catch (Throwable e) { }
      }
      else
      {
        info.localVersion = -1;
        
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        String mapFile = settings.getString("map_file", "");
        if (mapFile.equals(info.archiveFile))
        {
          NavigineApp.Navigation.loadArchive(null);
          SharedPreferences.Editor editor = settings.edit();
          editor.putString("map_file", "");
          editor.commit();
        }
      }
    }
    mAdapter.updateList();
  }
  
  private void updateLoader()
  {
    Log.d(TAG, String.format(Locale.ENGLISH, "Update loader: %d", mLoader));
    
    long timeNow = DateTimeUtils.currentTimeMillis();
    if (mLoader < 0)
      return;
    
    int status = LocationLoader.checkLocationLoader(mLoader);
    if (status < 100)
    {
      if ((Math.abs(timeNow - mLoaderTime) > LOADER_TIMEOUT / 3 && status == 0) ||
          (Math.abs(timeNow - mLoaderTime) > LOADER_TIMEOUT))
      {
        mListView.setVisibility(LinearLayout.GONE);
        mStatusLabel.setVisibility(LinearLayout.VISIBLE);
        mStatusLabel.setText("Loading timeout!\nPlease, check your internet connection!");
        Log.d(TAG, String.format(Locale.ENGLISH, "Load stopped on timeout!"));
        LocationLoader.stopLocationLoader(mLoader);
        mLoader = -1;
      }
      else
      {
        mListView.setVisibility(LinearLayout.GONE);
        mStatusLabel.setVisibility(LinearLayout.VISIBLE);
        mStatusLabel.setText((CharSequence)String.format(Locale.ENGLISH, "Loading content (%d%%)", status));
      }
    }
    else
    {
      Log.d(TAG, String.format(Locale.ENGLISH, "Load finished with result: %d", status));
      LocationLoader.stopLocationLoader(mLoader);
      mLoader = -1;
      
      if (status == 100)
      {
        parseMapsXml();
        if (mInfoList.isEmpty())
        {
          mListView.setVisibility(LinearLayout.GONE);
          mStatusLabel.setVisibility(LinearLayout.VISIBLE);
          mStatusLabel.setText("No locations available");
        }
        else
        {
          mListView.setVisibility(LinearLayout.VISIBLE);
          mStatusLabel.setVisibility(LinearLayout.GONE);
        }
      }
      else
      {
        mListView.setVisibility(LinearLayout.GONE);
        mStatusLabel.setVisibility(LinearLayout.VISIBLE);
        mStatusLabel.setText("Error loading!\nPlease, check your ID!");
      }
    }
  }
  
  long mUpdateLocationLoadersTime = 0;
  private void updateLocationLoaders()
  {
    long timeNow = DateTimeUtils.currentTimeMillis();
    mUpdateLocationLoadersTime = timeNow;
    
    synchronized (mLoaderMap)
    {
      Iterator<Map.Entry<String,LoaderState> > iter = mLoaderMap.entrySet().iterator();
      while (iter.hasNext())
      {
        Map.Entry<String,LoaderState> entry = iter.next();
        
        LoaderState loader = entry.getValue();
        if (loader.state < 100)
        {
          loader.timeLabel = timeNow;
          if (loader.type == DOWNLOAD)
            loader.state = LocationLoader.checkLocationLoader(loader.id);
          if (loader.type == UPLOAD)
            loader.state = LocationLoader.checkLocationUploader(loader.id);
        }
        else if (loader.state == 100)
        {
          String archivePath = NavigineApp.Navigation.getArchivePath();
          String locationFile = LocationLoader.getLocationFile(mApplicationContext, loader.location);
          if (archivePath != null && archivePath.equals(locationFile))
          {
            Log.d(TAG, "Reloading archive " + archivePath);
            if (NavigineApp.Navigation.loadArchive(archivePath))
            {
              SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
              SharedPreferences.Editor editor = settings.edit();
              editor.putString("map_file", archivePath);
              editor.commit();
            }
          }
          if (loader.type == DOWNLOAD)
            LocationLoader.stopLocationLoader(loader.id);
          if (loader.type == UPLOAD)
            LocationLoader.stopLocationUploader(loader.id);
          iter.remove();
        }
        else
        {
          // Load failed
          if (Math.abs(timeNow - loader.timeLabel) > 5000)
          {
            if (loader.type == DOWNLOAD)
              LocationLoader.stopLocationLoader(loader.id);
            if (loader.type == UPLOAD)
              LocationLoader.stopLocationUploader(loader.id);
            iter.remove();
          }
        }
      }
    }
    updateLocalVersions();
    mAdapter.updateList();
  }
  
  private void parseMapsXml()
  {
    try
    {
      String fileName = LocationLoader.getLocationDir(mApplicationContext, null) + "/maps.xml";
      List<LocationInfo> infoList = Parser.parseMapsXml(mApplicationContext, fileName);
      mInfoList = new ArrayList<LocationInfo>();
      if (infoList != null)
        mInfoList = infoList;
      mAdapter.updateList();
      new File(fileName).delete();
    }
    catch (Throwable e)
    {
      Log.e(TAG, Log.getStackTraceString(e));
    }
  }
  
  private void startDownload(int index)
  {
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    String userHash = settings.getString("user_hash", "");
    if (userHash.length() == 0)
      return;
    
    LocationInfo info = mInfoList.get(index);
    String location = new String(info.title);
    Log.d(TAG, String.format(Locale.ENGLISH, "Start download: %s", location));
    
    synchronized (mLoaderMap)
    {
      if (!mLoaderMap.containsKey(location))
      {
        LoaderState loader = new LoaderState();
        loader.location = location;
        loader.type = DOWNLOAD;
        loader.id = LocationLoader.startLocationLoader(userHash, location, info.archiveFile, true);
        mLoaderMap.put(location, loader);
      }
    }
    mAdapter.updateList();
  }
  
  private void startUpload(int index)
  {
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    String userHash = settings.getString("user_hash", "");
    if (userHash.length() == 0)
      return;
    
    LocationInfo info = mInfoList.get(index);
    String location = new String(info.title);
    Log.d(TAG, String.format(Locale.ENGLISH, "Start upload: %s", location));
    
    synchronized (mLoaderMap)
    {
      if (!mLoaderMap.containsKey(location))
      {
        LoaderState loader = new LoaderState();
        loader.location = location;
        loader.type = UPLOAD;
        loader.id = LocationLoader.startLocationUploader(userHash, location, info.archiveFile, true);
        mLoaderMap.put(location, loader);
      }
    }
    mAdapter.updateList();
  }
  
  final Runnable mRunnable =
    new Runnable()
    {
      public void run()
      {
        long timeNow = DateTimeUtils.currentTimeMillis();
        
        SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
        String userHash = settings.getString("user_hash", "");
        if (userHash.length() == 0)
          return;
        
        if (mLoader >= 0)
          updateLoader();
        
        if (Math.abs(timeNow - mUpdateLocationLoadersTime) > 1000)
          updateLocationLoaders();
      }
    };
}

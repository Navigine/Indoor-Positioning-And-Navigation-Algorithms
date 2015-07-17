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

public class NavigationActivity extends Activity
{
  // Constants
  private static final String TAG = "Navigine.NavigationActivity";
  private static final int UPDATE_TIMEOUT = 100;
  private static final float EPS = 0.000001f;
  
  // This context
  private Context mContext = this;
  private Context mApplicationContext = null;
  
  // GUI parameters
  private ImageView  mImageView    = null;
  private ImageView  mScaleView    = null;
  private ImageView  mIconsView    = null;
  private Button     mPrevFloorButton = null;
  private Button     mNextFloorButton = null;
  private TextView   mCurrentFloorTextView = null;
  private TimerTask  mTimerTask    = null;
  private Timer      mTimer        = new Timer();
  private Handler    mHandler      = new Handler();
  private boolean    mAdjustMode   = true;
  private boolean    mDrawScale    = true;
  
  // Image parameters
  int mMapWidth    = 0;
  int mMapHeight   = 0;
  int mViewWidth   = 0;
  int mViewHeight  = 0;
  RectF mMapRect   = null;
  Drawable mMapDrawable = null;
  PictureDrawable mPicDrawable = null;
  LayerDrawable mDrawable = null;
  
  // Multi-touch parameters
  private static final int TOUCH_MODE_SCROLL = 1;
  private static final int TOUCH_MODE_ZOOM   = 2;
  private static final int TOUCH_MODE_ROTATE = 3;
  private static final int TOUCH_SENSITIVITY = 10;
  private int mTouchMode = 0;
  private int mTouchLength = 0;
  private long mTouchTimeout = 0;
  private PointF[] mTouchPoints = new PointF[] { new PointF(0.0f, 0.0f),
                                                 new PointF(0.0f, 0.0f),
                                                 new PointF(0.0f, 0.0f)};
  
  // Geometry parameters
  private Matrix  mMatrix        = null;
  private float   mRatio         = 1.0f;
  private float   mAdjustAngle   = 0.0f;
  private long    mAdjustTime    = 0;
  private long    mAdjustTimeout = 7000;
  
  // Config parameters
  private float   mMinX = 0.0f;
  private float   mMaxX = 0.0f;
  private float   mMinY = 0.0f;
  private float   mMaxY = 0.0f;
  private float   mMinRatio = 0.1f;
  private float   mMaxRatio = 10.0f;
  
  // Device parameters
  private List<DeviceInfo> mDevices = null;   // List of all devices
  private DeviceInfo mCurrentDevice = null;   // Current device
  private LocationPoint mTargetPoint = null;  // Current device target
  
  // Location parameters
  private Location mLocation = null;
  private int mCurrentSubLocationIndex = -1;
  
  private boolean mBackgroundActiveMode = true;
  
  /** Called when the activity is first created */
  @Override public void onCreate(Bundle savedInstanceState)
  {
    Log.d(TAG, "NavigationActivity created");
    Log.d(TAG, String.format(Locale.ENGLISH, "Android API LEVEL: %d",
          android.os.Build.VERSION.SDK_INT));
    
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    setContentView(R.layout.navigation);
    
    mApplicationContext = getApplicationContext();
    
    // Setting up GUI parameters
    mImageView = (ImageView)findViewById(R.id.map_image_view);
    mScaleView = (ImageView)findViewById(R.id.scale_image_view);
    mIconsView = (ImageView)findViewById(R.id.icons_image_view);
    mPrevFloorButton = (Button)findViewById(R.id.navigation_prev_floor_button);
    mNextFloorButton = (Button)findViewById(R.id.navigation_next_floor_button);
    mCurrentFloorTextView = (TextView)findViewById(R.id.navigation_current_floor_text_view);
    mPrevFloorButton.setVisibility(LinearLayout.INVISIBLE);
    mNextFloorButton.setVisibility(LinearLayout.INVISIBLE);
    
    mImageView.setBackgroundColor(Color.argb(255, 235, 235, 235));
    mScaleView.setImageBitmap(Bitmap.createBitmap(100, 30, Bitmap.Config.ARGB_8888));
    mIconsView.setImageBitmap(Bitmap.createBitmap(100, 30, Bitmap.Config.ARGB_8888));
    
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    mBackgroundActiveMode = settings.getBoolean("background_active_mode", true);
    
    // Setting up touch listener
    mImageView.setOnTouchListener(
      new OnTouchListener()
      {
        @Override public boolean onTouch(View v, MotionEvent event)
        {
          doTouch(event);
          return true;
        }
      });
    
    mPrevFloorButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          if (loadPrevSubLocation())
            mAdjustTime = DateTimeUtils.currentTimeMillis() + mAdjustTimeout;
        }
      });
    
    mNextFloorButton.setOnClickListener(
      new OnClickListener()
      {
        @Override public void onClick(View v)
        {
          if (loadNextSubLocation())
            mAdjustTime = DateTimeUtils.currentTimeMillis() + mAdjustTimeout;
        }
      });
    
    if (mBackgroundActiveMode)
    {
      NavigineApp.setTrackFile();
      NavigineApp.Navigation.work();
      NavigineApp.Navigation.setScanTimeouts(1000, 100);
    }
  }
  
  @Override public void onDestroy()
  {
    super.onDestroy();
    
    if (mBackgroundActiveMode)
    {
      NavigineApp.Navigation.idle();
      Log.d(TAG, "Disconnecting from IMU");
      disconnectFromIMU();
    }
  }
  
  @Override public void onStart()
  {
    super.onStart();
    
    if (!mBackgroundActiveMode)
    {
      NavigineApp.setTrackFile();
      NavigineApp.Navigation.work();
      NavigineApp.Navigation.setScanTimeouts(1000, 100);
    }
    
    // Starting interface updates
    mTimerTask = 
      new TimerTask()
      {
        @Override public void run() 
        {
          mHandler.post(mRunnable);
        }
      };
    mTimer.schedule(mTimerTask, 500, UPDATE_TIMEOUT);
  }
  
  @Override public void onStop()
  {
    super.onStop();
    
    if (!mBackgroundActiveMode)
    {
      NavigineApp.Navigation.idle();
      Log.d(TAG, "Disconnecting from IMU");
      disconnectFromIMU();
    }
    
    mTimerTask.cancel();
    mTimerTask = null;
  }
  
  @Override public boolean onCreateOptionsMenu(Menu menu)
  {
    MenuInflater inflater = getMenuInflater();
    inflater.inflate(R.menu.navigation_menu, menu);
    menu.findItem(R.id.navigation_menu_tracking_on).setVisible(!mAdjustMode);
    menu.findItem(R.id.navigation_menu_tracking_off).setVisible(mAdjustMode);
    if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_NORMAL)
      menu.findItem(R.id.navigation_menu_disconnect_imu).setVisible(true);
    else
      menu.findItem(R.id.navigation_menu_disconnect_imu).setVisible(false);
    return true;
  }
  
  @Override public boolean onPrepareOptionsMenu(Menu menu)
  {
    menu.findItem(R.id.navigation_menu_tracking_on).setVisible(!mAdjustMode);
    menu.findItem(R.id.navigation_menu_tracking_off).setVisible(mAdjustMode);
    if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_NORMAL)
      menu.findItem(R.id.navigation_menu_disconnect_imu).setVisible(true);
    else
      menu.findItem(R.id.navigation_menu_disconnect_imu).setVisible(false);
    return true;
  }
  
  @Override public boolean onOptionsItemSelected(MenuItem item)
  {
    switch (item.getItemId())
    {
      case R.id.navigation_menu_tracking_on:
        startTracking();
        return true;
      
      case R.id.navigation_menu_tracking_off:
        stopTracking();
        return true;
      
      case R.id.navigation_menu_disconnect_imu:
        Log.d(TAG, "Disconnecting from IMU");
        disconnectFromIMU();
        return true;
      
      default:
        return super.onOptionsItemSelected(item);
    }
  }
  
  private void startTracking()
  {
    mAdjustMode = true;
    mAdjustTime = 0;
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    SharedPreferences.Editor editor = settings.edit();
    editor.putBoolean("adjust_mode", true);
    editor.commit();
  }
  
  private void stopTracking()
  {
    mAdjustMode = false;
    mAdjustTime = 0;
    SharedPreferences settings = mApplicationContext.getSharedPreferences("MainSettings", 0);
    SharedPreferences.Editor editor = settings.edit();
    editor.putBoolean("adjust_mode", false);
    editor.commit();
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
    
    mLocation = NavigineApp.Navigation.getLocation();
    mCurrentSubLocationIndex = -1;
    mMatrix = null;
    
    if (mLocation == null)
    {
      String text = "Load map failed: no location";
      Toast.makeText(mContext, (CharSequence)text, Toast.LENGTH_LONG).show();
      Log.e(TAG, text);
      return false;
    }
    
    if (mLocation.subLocations.size() == 0)
    {
      String text = "Load map failed: no sublocations";
      Toast.makeText(mContext, (CharSequence)text, Toast.LENGTH_LONG).show();
      Log.e(TAG, text);
      mLocation = null;
      return false;
    }
    
    if (!loadSubLocation(0))
    {
      String text = "Load map failed: unable to load default sublocation";
      Toast.makeText(mContext, (CharSequence)text, Toast.LENGTH_LONG).show();
      Log.e(TAG, text);
      mLocation = null;
      return false;
    }
    
    NavigineApp.setTrackFile();
    mHandler.post(mRunnable);
    return true;
  }
  
  private boolean loadSubLocation(int index)
  {
    if (mLocation == null || index < 0 || index >= mLocation.subLocations.size())
      return false;
    
    SubLocation subLoc = mLocation.subLocations.get(index);
    Log.d(TAG, String.format(Locale.ENGLISH, "Loading sublocation %s", subLoc.name));
    
    double[] gpsCoords = subLoc.getGpsCoordinates(0, 0);
    Log.d(TAG, String.format(Locale.ENGLISH, "GPS: (%.8f, %.8f)",
          gpsCoords[0], gpsCoords[1]));
    
    subLoc.getPicture();
    subLoc.getBitmap();
    
    if (subLoc.picture == null && subLoc.bitmap == null)
    {
      String text = "Load sublocation failed: invalid image";
      Toast.makeText(mContext, (CharSequence)text, Toast.LENGTH_LONG).show();
      Log.e(TAG, text);
      return false;
    }
    
    if (subLoc.width == 0.0f || subLoc.height == 0.0f)
    {
      String text = String.format(Locale.ENGLISH, "Load sublocation failed: invalid size: %.2f x %.2f",
                                  subLoc.width, subLoc.height);
      Toast.makeText(mContext, (CharSequence)text, Toast.LENGTH_LONG).show();
      Log.e(TAG, text);
      return false;
    }
    
    Log.d(TAG, String.format(Locale.ENGLISH, "Loading sublocation: %.2f x %.2f\n",
                             subLoc.width, subLoc.height));
    
    mViewWidth  = mImageView.getWidth();
    mViewHeight = mImageView.getHeight();
    Log.d(TAG, String.format(Locale.ENGLISH, "View size: %dx%d", mViewWidth, mViewHeight));
    
    // Updating image view size parameters
    float pixLength = 0.0f;
    if (mMatrix != null && mMapWidth > 0 && mRatio > 0)
      pixLength = (mMaxX - mMinX) / mMapWidth / mRatio; // Pixel length in meters
    
    // Determine absolute coordinates of the screen center
    PointF P = null;
    if (mMatrix != null)
      P = getAbsCoordinates(mViewWidth / 2, mViewHeight / 2);
    
    mMapWidth   = subLoc.picture == null ? subLoc.bitmap.getWidth()  : subLoc.picture.getWidth();
    mMapHeight  = subLoc.picture == null ? subLoc.bitmap.getHeight() : subLoc.picture.getHeight();
    mMapRect    = new RectF(0, 0, mMapWidth, mMapHeight);
    
    Log.d(TAG, String.format(Locale.ENGLISH, "Map size: %dx%d", mMapWidth, mMapHeight));
    
    mMapDrawable = subLoc.picture == null ? new BitmapDrawable(getResources(), subLoc.bitmap) : new PictureDrawable(subLoc.picture);
    mPicDrawable = new PictureDrawable(new Picture());
    
    Drawable[] drawables = {mMapDrawable, mPicDrawable};
    mDrawable = new LayerDrawable(drawables);
    mImageView.setImageDrawable(mDrawable);
    mImageView.setScaleType(ScaleType.MATRIX);
    
    // Reinitializing map/matrix parameters
    mMatrix      = new Matrix();
    mMaxX        = subLoc.width;
    mMaxY        = subLoc.height;
    mMinX        = 0.0f;
    mMinY        = 0.0f;
    mRatio       = 1.0f;
    mMinRatio    = subLoc.width / mMapWidth / 1.0f;
    mMaxRatio    = subLoc.width / mMapWidth / 0.01f;
    mAdjustAngle = 0.0f;
    mAdjustTime  = 0;
    mDrawScale   = true;
    
    // Calculating new pixel length in meters
    if (mMapWidth > 0 && pixLength > 0.0f)
      doZoom(subLoc.width / mMapWidth / pixLength);
    
    if (P != null)
    {
      PointF Q = getScreenCoordinates(P.x, P.y);
      doScroll(mViewWidth / 2 - Q.x, mViewHeight / 2 - Q.y);
    }
    else
    {
      doScroll(mViewWidth / 2 - mMapWidth / 2, mViewHeight / 2 - mMapHeight / 2);
      doZoom(Math.min((float)mViewWidth / mMapWidth, (float)mViewHeight / mMapHeight));
    }
    
    mCurrentSubLocationIndex = index;
    mCurrentFloorTextView.setText(String.format(Locale.ENGLISH, "%s.%s", mLocation.name, subLoc.name));
    mPrevFloorButton.setVisibility(mCurrentSubLocationIndex == 0 ? LinearLayout.INVISIBLE : LinearLayout.VISIBLE);
    mNextFloorButton.setVisibility(mCurrentSubLocationIndex == mLocation.subLocations.size() - 1 ? LinearLayout.INVISIBLE : LinearLayout.VISIBLE);
    mHandler.post(mRunnable);
    return true;
  }
  
  private boolean loadNextSubLocation()
  {
    if (mLocation == null || mCurrentSubLocationIndex < 0)
      return false;
    return loadSubLocation(mCurrentSubLocationIndex + 1);
  }
  
  private boolean loadPrevSubLocation()
  {
    if (mLocation == null || mCurrentSubLocationIndex < 0)
      return false;
    return loadSubLocation(mCurrentSubLocationIndex - 1);
  }
  
  private void doScroll(float deltaX, float deltaY)
  {
    if (mMatrix == null)
      return;
    //Log.d(TAG, String.format(Locale.ENGLISH, "Scroll by vector: (%.2f, %.2f)", deltaX, deltaY));
    float maxDeltaX = mViewWidth  / 2 - mMapRect.left;
    float minDeltaX = mViewWidth  / 2 - mMapRect.right;
    float maxDeltaY = mViewHeight / 2 - mMapRect.top;
    float minDeltaY = mViewHeight / 2 - mMapRect.bottom;
    //Log.d(TAG, String.format(Locale.ENGLISH, "Scroll bounds: dx: %.2f..%.2f, dy: %.2f..%.2f",
    //      minDeltaX, maxDeltaX, minDeltaY, maxDeltaY));
    deltaX = Math.max(Math.min(deltaX, maxDeltaX), minDeltaX);
    deltaY = Math.max(Math.min(deltaY, maxDeltaY), minDeltaY);
    
    mMatrix.postTranslate(deltaX, deltaY);
    mMatrix.mapRect(mMapRect, new RectF(0, 0, mMapWidth, mMapHeight));
    //Log.d(TAG, String.format(Locale.ENGLISH, "Map rect: (%.2f, %.2f) - (%.2f, %.2f)",
    //      mMapRect.left, mMapRect.top, mMapRect.right, mMapRect.bottom));
  }
  
  private void doZoom(float ratio)
  {
    if (mMatrix == null)
      return;
    //Log.d(TAG, String.format(Locale.ENGLISH, "Zoom by ratio: %.2f", ratio));
    float r = Math.max(Math.min(ratio, mMaxRatio / mRatio), mMinRatio / mRatio);
    mMatrix.postScale(r, r, mViewWidth / 2, mViewHeight / 2);
    mMatrix.mapRect(mMapRect, new RectF(0, 0, mMapWidth, mMapHeight));
    mRatio *= r;
    mDrawScale = true;
    //Log.d(TAG, String.format(Locale.ENGLISH, "Map rect: (%.2f, %.2f) - (%.2f, %.2f)",
    //      mMapRect.left, mMapRect.top, mMapRect.right, mMapRect.bottom));
  }
  
  private void doRotate(float angle, float x, float y)
  {
    if (mMatrix == null)
      return;
    //Log.d(TAG, String.format(Locale.ENGLISH, "Rotate: angle=%.2f, center=(%.2f, %.2f)", angle, x, y));
    float angleInDegrees = angle * 180.0f / (float)Math.PI;
    mMatrix.postRotate(angleInDegrees, x, y);
    mMatrix.mapRect(mMapRect, new RectF(0, 0, mMapWidth, mMapHeight));
    //Log.d(TAG, String.format(Locale.ENGLISH, "Map rect: (%.2f, %.2f) - (%.2f, %.2f)",
    //      mMapRect.left, mMapRect.top, mMapRect.right, mMapRect.bottom));
  }
  
  // Convert absolute coordinates (x,y) to SVG coordinates
  private PointF getSvgCoordinates(float x, float y)
  {
    return new PointF((x - mMinX) / (mMaxX - mMinX) * mMapWidth,
                      (mMaxY - y) / (mMaxY - mMinY) * mMapHeight);
  }
  
  private float getSvgLength(float d)
  {
    return Math.max(d * mMapWidth / (mMaxX - mMinX),
                    d * mMapHeight / (mMaxY - mMinY));
  }
  
  // Convert absolute coordinates (x,y) to screen coordinates
  private PointF getScreenCoordinates(float x, float y)
  {
    float[] pts = {(x - mMinX) / (mMaxX - mMinX) * mMapWidth,
                   (mMaxY - y) / (mMaxY - mMinY) * mMapHeight};
    mMatrix.mapPoints(pts);
    return new PointF(pts[0], pts[1]);
  }
  
  // Convert screen coordinates (x,y) to absolute coordinates
  private PointF getAbsCoordinates(float x, float y)
  {
    Matrix invMatrix = new Matrix();
    mMatrix.invert(invMatrix);
    
    float[] pts = {x, y};
    invMatrix.mapPoints(pts);
    return new PointF(pts[0] / mMapWidth  * (mMaxX - mMinX) + mMinX,
                     -pts[1] / mMapHeight * (mMaxY - mMinY) + mMaxY);
  }
  
  private void doTouch(MotionEvent event)
  {
    long timeNow = DateTimeUtils.currentTimeMillis();
    int actionMask = event.getActionMasked();
    int pointerIndex = event.getActionIndex();
    int pointerCount = event.getPointerCount();
    
    PointF[] points = new PointF[pointerCount];
    for(int i = 0; i < pointerCount; ++i)
      points[i] = new PointF(event.getX(i), event.getY(i));
    
    //Log.d(TAG, String.format(Locale.ENGLISH, "MOTION EVENT: %d", actionMask));
    
    if (actionMask == MotionEvent.ACTION_DOWN)
    {
      mTouchPoints[0].set(points[0]);
      mTouchTimeout = timeNow + 500;
      mTouchLength = 0;
      mTouchMode = 0;
      return;
    }
    
    if (actionMask != MotionEvent.ACTION_MOVE)
    {
      mTouchTimeout = 0;
      mTouchMode = 0;
      return;
    }
    
    // Handling move events
    switch (pointerCount)
    {
      case 1:
        if (mTouchMode == TOUCH_MODE_SCROLL)
        {
          float deltaX = points[0].x - mTouchPoints[0].x;
          float deltaY = points[0].y - mTouchPoints[0].y;
          mTouchLength += Math.abs(deltaX);
          mTouchLength += Math.abs(deltaY);
          if (mTouchLength > TOUCH_SENSITIVITY)
            mTouchTimeout = 0;
          
          doScroll(deltaX, deltaY);
          mAdjustTime = timeNow + mAdjustTimeout;
          mImageView.setImageMatrix(mMatrix);
          //mHandler.post(mRunnable);
        }
        mTouchMode = TOUCH_MODE_SCROLL;
        mTouchPoints[0].set(points[0]);
        break;
      
      case 2:
        if (mTouchMode == TOUCH_MODE_ZOOM)
        {
          float oldDist = PointF.length(mTouchPoints[0].x - mTouchPoints[1].x, mTouchPoints[0].y - mTouchPoints[1].y);
          float newDist = PointF.length(points[0].x - points[1].x, points[0].y - points[1].y);
          oldDist = Math.max(oldDist, 1.0f);
          newDist = Math.max(newDist, 1.0f);
          float ratio = newDist / oldDist;
          //ratio = (ratio + 1) / 2;
          doZoom(ratio);
          mImageView.setImageMatrix(mMatrix);
          //mHandler.post(mRunnable);
          drawScale();
        }
        mTouchMode = TOUCH_MODE_ZOOM;
        mTouchPoints[0].set(points[0]);
        mTouchPoints[1].set(points[1]);
        break;
    }
  }
  
  private void doLongTouch(float x, float y)
  {
    // Exec target popup dialog
    showTargetPopupDialog(getAbsCoordinates(x, y));
  }
  
  private TreeMap<String, Integer> mColorMap = new TreeMap<String, Integer>();
  private int getClientColor(String id)
  {
    if (mColorMap.containsKey(id))
      return mColorMap.get(id).intValue();
    
    Integer color = null;
    switch (mColorMap.size() % 6)
    {
      case 0: color = Color.argb(255, 255, 0, 0); break;
      case 1: color = Color.argb(255, 0, 255, 0); break;
      case 2: color = Color.argb(255, 0, 0, 255); break;
      case 3: color = Color.argb(255, 255, 255, 0); break;
      case 4: color = Color.argb(255, 255, 0, 255); break;
      case 5: color = Color.argb(255, 0, 255, 255); break;
    }
    mColorMap.put(id, color);
    return color.intValue();
  }
  
  private void drawDevice(DeviceInfo info, Canvas canvas)
  {
    // Ignoring Raspberry devices
    if (info.type.equals("raspberry"))
      return;
    
    // Check if location is loaded
    if (mLocation == null || mCurrentSubLocationIndex < 0)
      return;
    
    // Check if device belongs to the location loaded
    if (info.location != mLocation.id)
      return;
    
    // Get current sublocation displayed
    SubLocation subLoc = mLocation.subLocations.get(mCurrentSubLocationIndex);
    
    // Drawing device path (if it exists)
    if (info.path != null && info.path.length > 1)
    {
      Paint pathPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
      pathPaint.setStrokeWidth(3.0f);
      pathPaint.setStyle(Paint.Style.STROKE);
      
      pathPaint.setARGB(255, 0, 0, 255);
      
      for(int j = 1; j < info.path.length; ++j)
      {
        LocationPoint P = info.path[j-1];
        LocationPoint Q = info.path[j];
        
        if (P.subLocation == subLoc.id && Q.subLocation == subLoc.id)
          drawArrow(getSvgCoordinates(P.x, P.y),
                    getSvgCoordinates(Q.x, Q.y),
                    pathPaint, canvas);
      }
    }
    
    // Check if device belongs to the current sublocation
    if (info.subLocation != subLoc.id)
      return;
    
    float x = info.x;
    float y = info.y;
    float r = info.r;
    float angle = info.azimuth;
    float sinA = (float)Math.sin(angle);
    float cosA = (float)Math.cos(angle);
    float radius = getSvgLength(r);
    
    PointF P = getSvgCoordinates(x, y);
    PointF Q = getSvgCoordinates(x + r * sinA, y + r * cosA);
    PointF R = getSvgCoordinates(x + r * cosA * 0.66f - r * sinA * 0.25f, y - r * sinA * 0.66f - r * cosA * 0.25f);
    PointF S = getSvgCoordinates(x - r * cosA * 0.66f - r * sinA * 0.25f, y + r * sinA * 0.66f - r * cosA * 0.25f);
    
    // Preparing paints
    Paint strokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    Paint fillPaintSolid = new Paint(Paint.ANTI_ALIAS_FLAG);
    Paint fillPaintTransparent = new Paint(Paint.ANTI_ALIAS_FLAG);
    strokePaint.setStrokeWidth(0.0f);
    strokePaint.setStyle(Paint.Style.STROKE);
    fillPaintSolid.setStyle(Paint.Style.FILL);
    fillPaintTransparent.setStyle(Paint.Style.FILL);
    
    int solidColor = getClientColor(info.id);
    int textColor  = Color.argb(255, Color.red(solidColor) / 3, Color.green(solidColor) / 3, Color.blue(solidColor) / 3);
    int fillColor  = Color.argb(100, Color.red(solidColor), Color.green(solidColor), Color.blue(solidColor));
    strokePaint.setColor(textColor);
    fillPaintSolid.setColor(solidColor);
    fillPaintTransparent.setColor(fillColor);
    
    // Drawing circle
    canvas.drawCircle(P.x, P.y, radius, fillPaintTransparent);
    
    // Drawing orientation
    Path path = new Path();
    path.moveTo(Q.x, Q.y);
    path.lineTo(R.x, R.y);
    path.lineTo(P.x, P.y);
    path.lineTo(S.x, S.y);
    path.lineTo(Q.x, Q.y);
    canvas.drawPath(path, fillPaintSolid);
    canvas.drawPath(path, strokePaint);
  }
  
  private void adjustDevice(DeviceInfo info)
  {
    // Check if location is loaded
    if (mLocation == null || mCurrentSubLocationIndex < 0)
      return;
    
    // Check if device belongs to the location loaded
    if (info.location != mLocation.id)
      return;
    
    long timeNow = DateTimeUtils.currentTimeMillis();
    
    // Adjust map, if necessary
    if (timeNow >= mAdjustTime)
    {
      // Firstly, set the correct sublocation
      SubLocation subLoc = mLocation.subLocations.get(mCurrentSubLocationIndex);
      if (info.subLocation != subLoc.id)
      {
        for(int i = 0; i < mLocation.subLocations.size(); ++i)
          if (mLocation.subLocations.get(i).id == info.subLocation)
            loadSubLocation(i);
      }
      
      // Secondly, adjust device to the center of the screen
      PointF center = getScreenCoordinates(info.x, info.y);
      float deltaX  = mViewWidth  / 2 - center.x;
      float deltaY  = mViewHeight / 2 - center.y;
      doScroll(deltaX, deltaY);
      
      // Thirdly, adjust device direction to the top of screen
      //float angle = info.azimuth;
      //float deltaA = mAdjustAngle - angle;
      //doRotate(deltaA, center.x, center.y);
      //mAdjustAngle -= deltaA;
      
      //Log.d(TAG, String.format(Locale.ENGLISH, "Adjusted by: (%.2f, %.2f), %.2f (%.2f)",
      //      deltaX, deltaY, deltaA, angle));
      mAdjustTime = timeNow;
    }
  }
  
  private void drawScale()
  {
    if (mMatrix == null)
      return;
    
    // Preparing canvas
    Bitmap bitmap = ((BitmapDrawable)mScaleView.getDrawable()).getBitmap();
    bitmap.eraseColor(Color.TRANSPARENT);
    Canvas canvas = new Canvas(bitmap);
    
    // Calculate scale meter-length
    float length = 70.0f / mRatio / mMapWidth * (mMaxX - mMinX);
    String text = "";
    
    if (length < 0.1f)
      text = String.format(Locale.ENGLISH, "%d %s", Math.round(length * 100), getString(R.string.centimeters));
    else if (length >= 0.1f && length < 1.0f)
      text = String.format(Locale.ENGLISH, "%d %s", Math.round(length * 10) * 10, getString(R.string.centimeters));
    else if (length >= 1.0f && length < 2.0f)
      text = String.format(Locale.ENGLISH, "%.1f m", Math.round(length * 5) / 5.0f, getString(R.string.meters));
    else if (length >= 2.0f && length < 5.0f)
      text = String.format(Locale.ENGLISH, "%.1f m", Math.round(length * 2) / 2.0f, getString(R.string.meters));
    else if (length >= 5.0f && length < 10.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length), getString(R.string.meters));
    else if (length >= 10.0f && length < 20.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length / 2) * 2, getString(R.string.meters));
    else if (length >= 20.0f && length < 50.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length / 5) * 5, getString(R.string.meters));
    else if (length >= 50.0f && length < 100.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length / 10) * 10, getString(R.string.meters));
    else if (length >= 100.0f && length < 200.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length / 20) * 20, getString(R.string.meters));
    else if (length >= 200.0f && length < 500.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length / 50) * 50, getString(R.string.meters));
    else if (length >= 500.0f && length < 1000.0f)
      text = String.format(Locale.ENGLISH, "%d m", Math.round(length / 100) * 100, getString(R.string.meters));
    else if (length >= 1000.0f && length < 2000.0f)
      text = String.format(Locale.ENGLISH, "%.1f km", Math.round(length / 200) / 5.0f, getString(R.string.kilometers));
    else if (length >= 2000.0f && length < 5000.0f)
      text = String.format(Locale.ENGLISH, "%.1f km", Math.round(length / 500) / 2.0f, getString(R.string.kilometers));
    else
      text = String.format(Locale.ENGLISH, "%d km", Math.round(length / 1000), getString(R.string.kilometers));
    
    // Draw text
    Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    paint.setStyle(Paint.Style.STROKE);
    paint.setColor(Color.BLACK);
    paint.setStrokeWidth(0);
    
    Paint fillPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    fillPaint.setStyle(Paint.Style.FILL);
    fillPaint.setColor(Color.BLACK);
    
    canvas.drawRect(10, 18, 80, 28, fillPaint);
    fillPaint.setColor(Color.WHITE);
    canvas.drawRect(24, 18, 38, 28, fillPaint);
    canvas.drawRect(52, 18, 66, 28, fillPaint);
    
    float textWidth = paint.measureText(text);
    canvas.drawText(text, 45 - textWidth / 2, 15, paint);
    canvas.drawRect(10, 18, 80, 28, paint);
  }
  
  private void drawArrow(PointF A, PointF B, Paint paint, Canvas canvas)
  {
    float ux = B.x - A.x;
    float uy = B.y - A.y;
    float n = (float)Math.sqrt(ux * ux + uy * uy);
    float m = Math.min(15.0f, n / 3);
    float k = m / n;
    
    PointF C = new PointF(k * A.x + (1 - k) * B.x, k * A.y + (1 - k) * B.y);
    
    float wx = -uy * m / n;
    float wy =  ux * m / n;
    
    PointF E = new PointF(C.x + wx / 3, C.y + wy / 3);
    PointF F = new PointF(C.x - wx / 3, C.y - wy / 3);
    
    Path path = new Path();
    path.moveTo(B.x, B.y);
    path.lineTo(E.x, E.y);
    path.lineTo(F.x, F.y);
    path.lineTo(B.x, B.y);
    
    canvas.drawLine(A.x, A.y, B.x, B.y, paint);
    canvas.drawPath(path, paint);
  }
  
  private PointF _targetPoint = null;
  private void showTargetPopupDialog(PointF P)
  {
    // Check if location is loaded
    if (mLocation == null || mCurrentSubLocationIndex < 0)
      return;
    
    _targetPoint = P;
    
    LayoutInflater inflater = getLayoutInflater();
    View view = inflater.inflate(R.layout.target_point_dialog, null);
    String title = String.format(Locale.ENGLISH, "Target point (%.2f, %.2f)", P.x, P.y);
    TextView textView = (TextView)view.findViewById(R.id.target_point_dialog_description);
    textView.setText("Make route to point?");
    
    AlertDialog.Builder alertBuilder = new AlertDialog.Builder(mContext);
    alertBuilder.setView(view);
    alertBuilder.setTitle(title);
    
    alertBuilder.setPositiveButton(getString(R.string.make_route),
      new DialogInterface.OnClickListener()
      {
        @Override public void onClick(DialogInterface dlg, int id)
        {
          // Get the current sub-location
          SubLocation subLoc = mLocation.subLocations.get(mCurrentSubLocationIndex);
          if (NavigineApp.Navigation != null && subLoc != null)
          {
            mTargetPoint = new LocationPoint(subLoc.id, _targetPoint.x, _targetPoint.y);
            NavigineApp.Navigation.setTarget(mTargetPoint);
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
    
    if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_IDLE)
    {
      alertBuilder.setNeutralButton("Connect IMU",
        new DialogInterface.OnClickListener()
        {
          @Override public void onClick(DialogInterface dlg, int id)
          {
            SubLocation subLoc = mLocation.subLocations.get(mCurrentSubLocationIndex);
            if (subLoc != null)
              connectToIMU(subLoc.id, _targetPoint.x, _targetPoint.y);
            dlg.cancel();
          }
        });
    }
    
    AlertDialog alertDialog = alertBuilder.create();
    alertDialog.setCanceledOnTouchOutside(false);
    alertDialog.show();
  }
  
  private String getIMULogFile()
  {
    if (NavigineApp.Navigation == null)
      return "";
    
    String arhivePath = NavigineApp.Navigation.getArchivePath();
    if (arhivePath != null && arhivePath.length() > 0)
    {
      for(int i = 1; true; ++i)
      {
        String suffix = String.format(Locale.ENGLISH, ".IMU.%d.log", i);
        String filename = arhivePath.replaceAll("\\.zip$", suffix);
        if (!(new File(filename)).exists())
          return filename;
      }
    }
    return "";
  }
  
  private void connectToIMU(int subLocId, float x0, float y0)
  {
    if (mLocation == null)
      return;
    
    SubLocation subLoc = mLocation.getSubLocation(subLocId);
    if (subLoc == null)
      return;
    
    if (NavigineApp.Navigation != null)
      NavigineApp.IMU.setLogFile(getIMULogFile());
    
    float a0 = -subLoc.azimuth * (float)Math.PI / 180.0f;
    if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_IDLE)
      NavigineApp.IMU.connect(mLocation.id, subLocId, x0, y0, a0);
  }
  
  private void disconnectFromIMU()
  {
    if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_NORMAL)
      NavigineApp.IMU.disconnect();
  }
  
  private boolean mMapLoaded = false;
  private boolean mImuMode   = false;
  private long mPopupTimeout = 60000;
  private long mPopupTime = 0;
  private float _theta = 0.0f;
  
  final Runnable mRunnable =
    new Runnable()
    {
      public void run()
      {
        if (mMatrix == null)
        {
          if (!mMapLoaded && NavigineApp.Navigation.getArchivePath() != null)
          {
            loadMap(NavigineApp.Navigation.getArchivePath());
            mMapLoaded = true;
          }
          return;
        }
        
        if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_NORMAL)
        {
          if (!mImuMode)
          {
            mImuMode = true;
            NavigineApp.Navigation.idle();
          }
        }
        else
        {
          if (mImuMode)
          {
            mImuMode = false;
            NavigineApp.setTrackFile();
            NavigineApp.Navigation.work();
          }
        }
        
        long timeNow = DateTimeUtils.currentTimeMillis();
        
        // Drawing scale, if necessary
        if (mDrawScale)
        {
          drawScale();
          mDrawScale = false;
        }
        
        // Handling long touch gesture
        if (mTouchTimeout > 0 && timeNow >= mTouchTimeout)
        {
          Log.d(TAG, String.format(Locale.ENGLISH, "Long click at (%.2f, %.2f)",
                mTouchPoints[0].x, mTouchPoints[0].y));
          mTouchTimeout = 0;
          doLongTouch(mTouchPoints[0].x, mTouchPoints[0].y);
        }
        
        Picture pic = mPicDrawable.getPicture();
        Canvas canvas = pic.beginRecording(mMapWidth, mMapHeight);

        mCurrentDevice = null;
        int errorCode = 0;
        
        if (NavigineApp.IMU.getConnectionState() == IMUThread.STATE_NORMAL)
        {
          mDevices = new ArrayList<DeviceInfo>();
          DeviceInfo device = NavigineApp.IMU.getDevice();
          if (device != null)
            mDevices.add(device);
        }
        else
        {
          mDevices = NavigineApp.Navigation.getDeviceList();
          errorCode = NavigineApp.Navigation.getErrorCode();
        }
        
        //Log.d(TAG, String.format(Locale.ENGLISH, "Error code = %d\n", errorCode));
        
        if (mDevices != null)
        {
          for(int i = 0; i < mDevices.size(); ++i)
          {
            DeviceInfo info = mDevices.get(i);
            //if (info.id.equals(NavigineApp.Navigation.getDeviceId()))
              mCurrentDevice = new DeviceInfo(info);
          }
        }
        
        if (mDevices != null)
        {
          for(int i = 0; i < mDevices.size(); ++i)
          {
            DeviceInfo info = mDevices.get(i);
            drawDevice(info, canvas);
          }
        }
        
        if (mAdjustMode && mCurrentDevice != null)
          adjustDevice(mCurrentDevice);
        
        //else if (Math.abs(mAdjustAngle) > EPS)
        //{
        //  // Restore normal map orientation
        //  doRotate(mAdjustAngle, mViewWidth / 2, mViewHeight / 2);
        //  mAdjustAngle = 0.0f;
        //}
        
        pic.endRecording();
        
        mImageView.invalidate();
        mImageView.setImageMatrix(mMatrix);
      }
    };
}

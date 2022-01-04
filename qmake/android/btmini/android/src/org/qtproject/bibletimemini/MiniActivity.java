/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

package org.qtproject.bibletimemini;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.os.Vibrator;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import java.util.Timer;
import java.util.TimerTask;

public class MiniActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static MiniActivity _instance;
//    private static Timer _keepTimer;
//    private static View.OnTouchListener _keepListener;

    public MiniActivity()
    {
        _instance = this;
//        _keepListener = new View.OnTouchListener() {
//            @Override
//            public boolean onTouch(View v, MotionEvent event)
//            {
//                _keepTimer = new Timer();
//                _keepTimer.schedule(new TimerTask() {
//                    @Override
//                    public void run() {
//                        Log.v("BtMini", "FLAG_KEEP_SCREEN_OFF 2");
//                        //_instance.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//                    }
//                }, seconds * 10);
//            }
//        }
    }

    public static void vibrate(long miliseconds)
    {
        Vibrator v = (Vibrator)_instance.getSystemService(Context.VIBRATOR_SERVICE);
        v.vibrate(miliseconds);
    }

    // Control keep screen on timeout.
//    public static void keepScreenAwake(long seconds)
//    {
//        if (seconds == 0)
//        {
//            _keepTimer = null;
//            Log.v("BtMini", "FLAG_KEEP_SCREEN_ON");
//            //_instance.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//        }
//        else if (seconds < 0)
//        {
//            _keepTimer = null;
//            Log.v("BtMini", "FLAG_KEEP_SCREEN_OFF");
//            //_instance.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//        }
//        else
//        {
//            // attach touch listener on first call
//            // start timer or call listener routine
//            // set flag on

//            _keepTimer = new Timer();
//            _keepTimer.schedule(new TimerTask() {
//                @Override
//                public void run() {
//                    Log.v("BtMini", "FLAG_KEEP_SCREEN_OFF 2");
//                    //_instance.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//                }
//            }, seconds * 10);

//            Log.v("BtMini", "FLAG_KEEP_SCREEN_ON 2");
//            //_instance.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//            _instance.getWindow().setOnClickListener(_keepListener);
//        }
//    }
}

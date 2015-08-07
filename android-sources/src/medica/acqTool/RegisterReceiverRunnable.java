package medica.acqTool;

import android.app.Activity;
import android.content.Intent;
import android.content.IntentFilter;

/**
 * Created by Luca on 04/08/2015.
 */
public class RegisterReceiverRunnable implements Runnable
{
    private Activity m_activity;
    public RegisterReceiverRunnable(Activity activity) {
        m_activity = activity;
    }
    // this method is called on Android Ui Thread
    @Override
    public void run() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_POWER_DISCONNECTED);
        filter.addAction(Intent.ACTION_POWER_CONNECTED);
        filter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);

        // this method must be called on Android Ui Thread
        m_activity.registerReceiver(new AcqToolReceiver(), filter);
    }
}

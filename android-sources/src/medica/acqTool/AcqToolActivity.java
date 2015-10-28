package medica.acqTool;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.app.Notification;
import android.app.NotificationManager;

import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class AcqToolActivity extends QtActivity
{

    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static AcqToolActivity m_instance;
    public AcqToolActivity()
    {
      m_instance=this;
    }

    // this method is called by C++ to register the BroadcastReceiver.
    public void registerBroadcastReceiver() {
        // Qt is running on a different thread than Android.
        // In order to register the receiver we need to execute it in the Android UI thread
        //Debug.waitForDebugger();
        runOnUiThread(new RegisterReceiverRunnable(this));
    }

    public void showNotification(String title,String message)
    {
        if (m_notificationManager == null) {
            m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
            m_builder = new Notification.Builder(m_instance);
            m_builder.setSmallIcon(R.drawable.icon);
            m_builder.setContentTitle(title);
        }

        m_builder.setContentText(message);
        m_notificationManager.notify(1, m_builder.build());

    }

    public void showToast(String toast)
    {
        final String message=toast;
        runOnUiThread(new Runnable() {
                          @Override
                          public void run() {
                              Toast.makeText(getApplicationContext(), message, Toast.LENGTH_LONG).show();
                          }
                      }
        );
    }

    public void launchService(String serviceAction)
    {
        Intent intent=new Intent(serviceAction);
        ComponentName c = startService(intent);
    }



    public boolean startApplication(String packageName)
    {
        Intent launchIntent = new Intent(getPackageManager().getLaunchIntentForPackage(packageName));
            try {
                startActivity(launchIntent);
            } catch (ActivityNotFoundException err) {
                Toast t = Toast.makeText(getApplicationContext(),
                        "App not found", Toast.LENGTH_SHORT);
                t.show();
                return false;
            }
        return true;
    }
}
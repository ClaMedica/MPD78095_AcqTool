package medica.acqTool;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;
import medica.acqTool.R;

public class AcqToolReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        // call the native method when it receives a new notification

        if (intent.getAction().equals(Intent.ACTION_POWER_CONNECTED)) {
            Toast.makeText(context,"Alimentazione attaccata",Toast.LENGTH_LONG).show();
            NativeFunctions.onNativePowerConnected();
        }
        else if (intent.getAction().equals(Intent.ACTION_POWER_DISCONNECTED)) {
            Toast.makeText(context,"Alimentazione staccata" , Toast.LENGTH_LONG).show();
            NativeFunctions.onNativePowerDisconnected();
        }
        else if (intent.getAction().equals(Intent.ACTION_AIRPLANE_MODE_CHANGED)) {
            Toast.makeText(context,"Modalita' aereo",Toast.LENGTH_LONG).show();
            NativeFunctions.onNativeAirplaneModeChanged();
        }
    }
}
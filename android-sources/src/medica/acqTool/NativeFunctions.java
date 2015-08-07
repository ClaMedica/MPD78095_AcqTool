package medica.acqTool;

public class NativeFunctions {
    // define the native function
    // these functions are called by the BroadcastReceiver object
    // when it receives a new notification
    public static native void onNativePowerConnected();
    public static native void onNativePowerDisconnected();
    public static native void onNativeAirplaneModeChanged();
}
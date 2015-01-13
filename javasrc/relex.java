
import java.io.*;
import java.util.*;
import java.text.*;


public class relex {

    public static relex x = null;
    
    public native void javainit();

    public native String comm(String command);
    static {
        System.loadLibrary("relex");
    }
}

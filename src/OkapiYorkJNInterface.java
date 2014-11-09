/**
 *  This is the okapi JNI interface to accessing okapi's native C interface
 *  in Java.
 *
 *  It serves as a close link to the okapi engine, thus, used in the most
 *  part for passing arguments into the okapi C api providing a very
 *  low level interface design.
 *
 *  The library generated from this can then be used by basic search
 *  engines which will need to worry about the actual setting up and
 *  taking down of the okapi environment that is created.
 *
 *  @author Mladen Kovacevic
 *  @date   30.Oct.2007
 */


public class OkapiYorkJNInterface
{

  /* The C api interface to calling okapi's engine */
  public native String i0(String s);

  /* The C api interface to initializing okapi's engine */
  public native void iinit();

  /* The C api interface to cleaning up okapi's engine */
  public native void iexit();
  
  static
  {
    System.loadLibrary("OkapiYorkInterface");
  }

  /* Main method to test out the interface itself */
  public static void main (String args[])
  {
    OkapiYorkJNInterface okapi = new OkapiYorkJNInterface();
    // Initialize the interface environment
    okapi.iinit();
    
    // Call okapi functions
    String s = "choose blog07";
    printCommandLine(s, okapi.i0(s));

    s = "sp t=heart attack thrombosis cardiac arrest";
    printCommandLine(s, okapi.i0(s));

    s = "f t=heart";
    printCommandLine(s, okapi.i0(s));

    s = "w fn=2 r=0 bigr=0 rload=4 bigrload=5 n=22";
    printCommandLine(s, okapi.i0(s));

    s = "f t=attack";
    printCommandLine(s, okapi.i0(s));

    s = "w fn=2 r=0 bigr=0 rload=4 bigrload=5 n=8";
    printCommandLine(s, okapi.i0(s));

    s = "f t=thrombosis";
    printCommandLine(s, okapi.i0(s));

    s = "w fn=2 r=0 bigr=0 rload=4 bigrload=5 n=0";
    printCommandLine(s, okapi.i0(s));

    s = "f t=cardiac";
    printCommandLine(s, okapi.i0(s));

    s = "w fn=2 r=0 bigr=0 rload=4 bigrload=5 n=0";
    printCommandLine(s, okapi.i0(s));

    s = "f t=arrest";
    printCommandLine(s, okapi.i0(s));

    s = "w fn=2 r=0 bigr=0 rload=4 bigrload=5 n=4";
    printCommandLine(s, okapi.i0(s));

    s = "f s=0 w=14.959 s=1 w=15.932 s=2 w=0.0 s=3 w=0.0 s=4 w=16.568 op=bm25";
    printCommandLine(s, okapi.i0(s));

    // Cleanup okapi environment
    okapi.iexit();
  }

  /*
   * This static method just prints to the standard output the same way
   * that it would look as if you were running i1+ from the command line.
   * Its for easier testing of how it should look on the screen.
   * @input inputString the input string that was passed to okapi
   * @input outputString the output string that was returned from okapi
   */
  public static void printCommandLine (String inputString, String outputString)
  {
    System.out.println("U: " + inputString + "\n" + outputString);
    return;
  }
}

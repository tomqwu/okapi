public class JNIDemo
{
	public static relex okapi_interface;
	public static void main(String[] args)
	{
		okapi_interface = new relex();
		okapi_interface.javainit();
		String okapi_output = okapi_interface.comm("ch 09article");
		okapi_output = okapi_interface.comm("set a=kw");
		okapi_output = okapi_interface.comm("f t=method");
		okapi_output = okapi_interface.comm("s");
		System.out.println(okapi_output);
	}
}

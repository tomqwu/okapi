public class pair extends java.lang.Object{
	public String key;
	public Object value;
	pair(String k,Object v){
		key=k;
		value=v;
	}
	public String toString(){
	return key+"   "+(Double)value;
	}
}

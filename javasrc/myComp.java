import java.util.*;


public class myComp implements Comparator{


	myComp(){
	}

	public int compare(Object x,Object y){
		pair xx=(pair)x;
		pair yy=(pair)y;
		double x1=((Double)(xx.value)).doubleValue();
		double y1=((Double)(yy.value)).doubleValue();
		if(x1<y1) return 1;
		if(x1==y1) return 0;
		return -1;
	}	
}

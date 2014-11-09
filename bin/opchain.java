
import java.util.*;

public class opchain {
	public LinkedList chain;
	public boolean del;
	opchain(LinkedList ll,boolean ifdel){
	chain = ll;
	del = ifdel;	
	}
	
	public String toString(){           //HUBIN
		 return "OPCHAINOPCHAIN object LinkedList chain= " +  chain + "boolean del= " + del;
	 }
}

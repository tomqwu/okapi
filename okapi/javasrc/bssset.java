
import java.util.*;

public class bssset {
    public int setno;
    public int record_num;
    public float weight;
    public int real_num;
    public LinkedList combina;   //representing a concept of a string
    public float discount=0.0f;
     
    bssset(int set_number) {
        setno = set_number;
    }

    bssset(int set_number, int record_no, float wei) {
        setno = set_number;
        record_num = record_no;
        weight = wei;
    }

    bssset(int set_number, int record_no) {
        setno = set_number;
        record_num = record_no;
    }
  
    public void put_discount(float ff){
	discount = ff;
    }
    public void putreal_num(int h) {
        real_num = h;
    }

    public void putreal_comb(LinkedList ll1) {
        combina = ll1;
        
   /*     StringBuffer combi_rep = new StringBuffer();                      //added by hubin

        for (int i = 0; i < combina.size(); i++) {                        //added by hubin
            combi_rep.append((String) (combina.get(i)) + "+");            //added by hubin
        }                                                                //added by hubin
    */           
        System.out.println("BSSSETBSSSETBSSSET  putreal_comb() combina= "  +  combina.toString());  //added by hubin       
    }

    public void destroy(relex x) {
        // x.comm("delete s="+new Integer(setno));
        return;
    }

    //it called by "outputq" in "squery" class
    public String concept() {
        StringBuffer combi_rep = new StringBuffer();

        for (int i = 0; i < combina.size(); i++) {
            combi_rep.append((String) (combina.get(i)) + "+");
        }
        
        String conceptstring = discount==0.0f?combi_rep.toString():"$"+(new Float(discount))+"$ "+combi_rep.toString(); //added by hubin
        System.out.println("BSSSETBSSSETBSSSET  concept() conceptstring = "  +  conceptstring);                 //added by hubin
        return discount==0.0f?combi_rep.toString():"$"+(new Float(discount))+"$ "+combi_rep.toString();
        
    }
    
    public String rep() {
        return "s=" + new Integer(setno) + " w=" + new Float(weight) + " ";
    }
    
    public String toString() {   //modified by hubin
     /*   StringBuffer combi_rep = new StringBuffer();

        for (int i = 0; i < combina.size(); i++) {
            combi_rep.append((String) (combina.get(i)) + "+");
        }
        */
    	String objectString = "";
    	if(combina == null)
    	    objectString = "BSSSETBSSSET object: Set No=" + new Integer(setno) + ";;;   Total="
                        + new Integer(real_num) + ";;;   Weight=" + new Float(weight)
                         + ";;;   Combi= NULL!!!";
    	else
    		objectString = "BSSSETBSSSET object: Set No=" + new Integer(setno) + ";;;   Total="
                         + new Integer(real_num) + ";;;   Weight=" + new Float(weight)
                          + ";;;   Combi=" + combina.toString();
        return objectString;
    }

    

}


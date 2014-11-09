public class op_pair {
public subterm first;
public String first_txt;

	op_pair(subterm fst, String fst_txt){
		first = fst;
		first_txt = fst_txt;
	}

	 public String toString(){     //hubin
		 return "OP_PAIR_OP_PAIR object subterm first= " +  first + "String first_txt= " + first_txt;
	 }

}


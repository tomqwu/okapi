public class query {

	String term, NP, set_number, weight;	
	boolean top, syn;
	query left_node, right_node, ex_node1, ex_node2, ex_node3, ex_node4;
	
	public query() {
		term = NP = set_number = weight = "";
		top = false;
		syn = false;
		left_node = null;
		right_node = null;
		ex_node1 = null;
		ex_node2 = null;
		ex_node3 = null;
		ex_node4 = null;
	}
	
	void setTerm(String t) {
		term = t;
	}
	
	void setNP(String np) {
		NP = np;
	}
	
	void setNumber(String n) {
		set_number = n;
	}
	
	void setWeight(String w) {
		weight = w;
	}

	void setTop(boolean t) {
		top = t;
	}
	
	void setSyn(boolean s) {
		syn = s;
	}
	
	void setLeftNode(query ln) {
		left_node = ln;
	}
	
	void setRightNode(query rn) {
		right_node = rn;
	}
	
	void setExNode1(query e1) {
		ex_node1 = e1;
	}

	void setExNode2(query e2) {
		ex_node2 = e2;
	}

	void setExNode3(query e3) {
		ex_node3 = e3;
	}
	
	void setExNode4(query e4) {
		ex_node4 = e4;
	}
	
	String getTerm() {
		return term;
	}
	
	String getNP() {
		return NP;
	}
	
	String getSetNumber() {
		return set_number;
	}
	
	String getWeight() {
		return weight;
	}
	
	boolean getTop() {
		return top;
	}
	
	boolean getSyn() {
		return syn;
	}
	
	query getLeftNode() {
		return left_node;
	}
	
	query getRightNode() {
		return right_node;
	}
	
	
	query getExNode1() {
		return ex_node1;
	}

	query getExNode2() {
		return ex_node2;
	}

	query getExNode3() {
		return ex_node3;
	}	
	
	query getExNode4() {
		return ex_node4;
	}
	
	query getShallowCopy() {
		query a = new query();
		a.term = this.getTerm();
		a.NP = this.getNP();
		a.set_number = this.getSetNumber();
		a.weight = this.getWeight();
		a.top = this.getTop();
		return a;
	}
	
}
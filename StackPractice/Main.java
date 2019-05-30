import java.util.Stack;

public class Main {

	public static void main(String[] args) {
		System.out.println("isPalindrome(\"Not A Palindrome\") Evaluates to: " + isPalindrome("Not A Palindrome"));
		System.out.println("isPalindrome(\"racecar\") Evaluates to: " + isPalindrome("racecar"));
	}
	
	/**
	 * Determines whether a String is the same spelled forward as spelled backwards
	 * @return true if String is a palindrome
	 */
	public static boolean isPalindrome(String str) {
		Stack<Character> tempStack = new Stack<Character>();
		String tempString = "";
		
		for(int i=0; i<str.length(); i++)
			tempStack.push(str.charAt(i));
		for(int i=0; i<str.length(); i++)
			tempString += tempStack.pop();
		
		if(str.compareTo(tempString)==0) {
			return true;
		}
		else {
			return false;
		}
	}

}

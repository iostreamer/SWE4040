package ca.unb.backoffapp;

public class BackoffMessage {
	private String text;
	private String name;
	private double multiplier;
	
	/**
	 * Create a new message profile
	 * @param n - The name of the message profile, to be displayed in the list view
	 * @param t - The text of the message, which will be shown on the BackOff display.
	 * @param m - The multiplier factor of this profile.
	 */
	BackoffMessage(String n, String t, double m) {
		text = t;
		name = n;
		multiplier = m;
	}
	
	public String getText() {return text;}
	public String getName() {return name;}
	
}

/* add_port.c: add a port entry to the port file. This will get the
 *		info from a curses screen and write an entry to the
 *		port file.
 */

#include <string.h>
#include "uuinstall.h"

/* add_port merely calls up a template and then calls a function to
 * position the cursor to get data. If the user chooses to save the
 * entered data, it is saved to the port file.
 */

add_port()
{
	char z;
	FILE *portfd;

	port_template();
	z = get_port_data();
	if (z == 'y'){
		if( (portfd = fopen(PORTFILE,"a")) == NULL){
			mvwaddstr(portwin,7,5,"ERROR opening file for writing!    ");
			wrefresh(portwin);
			sleep(5);
			return;
		}
		wclear(portwin);
		mvwaddstr(portwin,12,32,"Adding entry...");
		wrefresh(portwin);
		sleep(1);

		fprintf(portfd, "\n%s\n",portname);
		fprintf(portfd, "%s\n", porttype);
		fprintf(portfd, "%s\n",portdev);
		fprintf(portfd, "%s\n",portspeed);

		if(strstr(porttype,"modem")){
			fprintf(portfd, "%s\n",portdial);
		}

		fclose(portfd);
	}
	wclear(portwin);
	wrefresh(portwin);
}


/* draw a template on the data entry screen */
port_template()
{
	wclear(portwin);
	mvwaddstr(portwin,9,10,"port");
	mvwaddstr(portwin,10,10,"type");
	mvwaddstr(portwin,11,10,"device");
	mvwaddstr(portwin,12,10,"baud");
	mvwaddstr(portwin,13,10,"dialer");

	wstandout(portwin);
	mvwaddstr(portwin,0,29,"Port File Entry Screen");
	mvwaddstr(portwin,9,17,"              ");
	mvwaddstr(portwin,10,17,"      ");
	mvwaddstr(portwin,11,17,"/dev/              ");
	mvwaddstr(portwin,12,17,"     ");
	mvwaddstr(portwin,13,17,"              ");
	wstandend(portwin);
	wrefresh(portwin);
}


/* get the data for this port */
get_port_data()
{

	char mdm;			/* flag for modem or direct connect */
	char * workstring;
	short true = 0;

	/* initialize the fields we will fill */
	strcpy(portname,"port ");
	strcpy(porttype,"type ");
	strcpy(portdev,"device /dev/");
	strcpy(portspeed,"baud ");
	strcpy(portdial,"dialer ");


	/* get a port name */
	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,20,0,"Enter the name that you want");
	mvwaddstr(portwin,21,0,"associated with  the  device");
	mvwaddstr(portwin,22,0,"that this entry will define.");
	mvwaddstr(portwin,23,0,"Enter nothing to cancel.");
	wrefresh(portwin);

	workstring = get_data(portwin,9,17,14,0,1);		/* get port name */
	if (strlen(workstring) == 0)
		return('n');

	strcat(portname,workstring);

	/* now find out if this is a direct or modem connection */

	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,20,0,"This  port  must  be  a  direct");
	mvwaddstr(portwin,21,0,"data line connection or used by");
	mvwaddstr(portwin,22,0,"a modem.  You must specify this.");
	wrefresh(portwin);

	strcpy(workstring,"");
	mvwaddstr(portwin,15,5,"Is this a <d>irect or <m>odem connection?");
	wrefresh(portwin);
	do{
		mvwaddstr(portwin,17,12,"Press 'd' or 'm'.");
		wrefresh(portwin);
		mdm = wgetch(portwin);
	}
	while ((mdm != 'd') && (mdm != 'm'));

	wmove(portwin, 10, 17);
	if (mdm == 'd'){
		wstandout(portwin);
		wprintw(portwin,"direct");
		wstandend(portwin);
		strcat(porttype,"direct");
	}else{
		wstandout(portwin);
		wprintw(portwin,"modem");
		wstandend(portwin);
		strcat(porttype,"modem");
	}

	wmove(portwin,15,4);
	wclrtoeol(portwin);
	wmove(portwin,17,1);
	wclrtoeol(portwin);
	wrefresh(portwin);
	strcpy(workstring,"");


	/* Now get the device associated with this port */

	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,20,0,"Enter  the name  of the  actual");
	mvwaddstr(portwin,21,0,"device. Examples: com2l, com1fl.");
	wrefresh(portwin);

	workstring = get_data(portwin,11,22,14,1,1);
	strcat(portdev,workstring);
	strcpy(workstring,"");

	/* now get the speed of this port */

	while(true != 1){

		wmove(portwin,20,0);
		wclrtobot(portwin);
		mvwaddstr(portwin,20,0,"Enter baud rate that communications");
		mvwaddstr(portwin,21,0,"will take place with via this port.");
		mvwaddstr(portwin,22,0,"Examples: 1200, 2400, 9600, 19200.");

		if (true == -1){
			wstandout(portwin);
			printf("\007");
			mvwaddstr(portwin,23,29,"Invalid speed entered!");
			wstandend(portwin);
		}
		wrefresh(portwin);

		workstring = get_data(portwin,12,17,5,1,1);
		true = validate_speed(atoi(workstring));
		strcat(portspeed,workstring);
		strcpy(workstring,"");
	}

	/* now get a dialer name. This will only be needed if
	 * this is a modem connection.
	 */

	if (strstr(porttype,"modem")){

		wmove(portwin,20,0);
		wclrtobot(portwin);
		mvwaddstr(portwin,20,0,"Enter the name of the dialer entry");
		mvwaddstr(portwin,21,0,"from  the dial file  that will  be");
		mvwaddstr(portwin,22,0,"used to dial this modem.");
		wrefresh(portwin);

		workstring = get_data(portwin,13,17,14,1,1);
		strcat(portdial, workstring);
		strcpy(workstring,"");
	}

	wmove(portwin,20,0);
	wclrtobot(portwin);
	mvwaddstr(portwin,7,5,"Do you wish to write this entry? (y/n)");
	wrefresh(portwin);
	do{
		mdm = wgetch(portwin);
	}
	while ((mdm != 'y') && (mdm != 'n'));

	wmove(portwin,7,1);
	wclrtoeol(portwin);
	wrefresh(portwin);
	return(mdm);
}

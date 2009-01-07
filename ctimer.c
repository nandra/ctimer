/***************************************************************************
 *   Copyright (C) 2009 by Marek Belisko                                   *
 *   marek.belisko@open-nandra.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int elapsed = 1;
int one_shot = 0;
char *exec = NULL;

/* signal handler */
void handler( signo )
{
	fprintf(stderr, "Timer expired\n");
	if (exec != NULL)
		system(exec);
	if (one_shot)
		elapsed = 0;
}

/* main */
int main(int argc, char *argv[])
{
 	int i = 0;
	timer_t t_id;
	struct itimerspec time_spec = {.it_interval= {.tv_sec=0,.tv_nsec=0},
					.it_value = {.tv_sec=0,.tv_nsec=0}};
	struct sigaction act;
	sigset_t set;

	int periodic = 0, time = 0, c;
		
	opterr = 0;
     
	while ((c = getopt (argc, argv, "hpot:e:")) != -1) {
		switch (c) {
           	case 'p':
             		periodic = 1;
             		break;
           	case 'o':
             		one_shot = 1;
             		break;
           	case 't':
             		time = atoi(optarg);
             		break;
		case 'e':
			exec = optarg;
			break;
		case 'h':
			fprintf(stderr, "Usage: ctimer <timer type options> [time] <path to program>\n\n");
			fprintf(stderr, "timer type options: \n");
			fprintf(stderr, " -p \tperiodic timer\n");
			fprintf(stderr, " -o \tone shot timer\n");
			fprintf(stderr, "time: \n");
			fprintf(stderr, " -t \time in seconds (default is 100s)\n");
			break;
           	case '?':
             		if (optopt == 't' || optopt == 'e')
               			fprintf (stderr, "Option -%c and requires an argument.\n", optopt);
             		else if (isprint (optopt))
               			fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             		else
               			fprintf (stderr,
                        		"Unknown option character `\\x%x'.\n", optopt);
             		goto exit;
           	default:
             		abort ();
           	}
	}
	
	if (periodic && one_shot) {
		fprintf(stderr, "Please select one shot or periodic timer only!!!\n");
		goto exit;
	}
	
	/* default timer is one shot */
	if (!periodic && !one_shot)
		one_shot = 1;
	
	/* default time */
	if (time == 0)
		time = 100;
			
	printf ("periodic = %d, one_shot = %d, time = %d, exec = %s\n",
        	periodic, one_shot, time, exec);

	sigemptyset( &set );
    	sigaddset( &set, SIGALRM );
	/* register signal handler */
	act.sa_flags = 0;
	act.sa_mask = set;
    	act.sa_handler = &handler;
    	
	sigaction( SIGALRM, &act, NULL );
	
	if (timer_create(CLOCK_MONOTONIC, NULL, &t_id)) 
		perror("timer_create");
	
	time_spec.it_value.tv_sec = time;
	
	if (periodic)
		time_spec.it_interval.tv_sec = time;
			
	if (timer_settime(t_id, 0, &time_spec, NULL))
		perror("timer_settime");
	
	/* neverending loop */
	while (elapsed);
			
	exit:
	return 0;
}

/*
 * wavemon - a wireless network monitoring aplication
 *
 * Copyright (c) 2001-2002 Jan Morgenstern <jan@jm-music.de>
 *
 * wavemon is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * wavemon is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with wavemon; see the file COPYING.  If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "conf.h"
#include "error.h"
#include "errno.h"
#include "iw_if.h"
#include "llist.h"

/* GLOBALS */
int conf_items;

char	*version_str = "wavemon wireless monitor 0.4.0b by Jan Morgenstern <jan@jm-music.de>",
	*license_str = "Distributed under the terms of the GNU General Public License >=v2.";

void help_exit()
{
	printf("%s\n%s\n\n", version_str, license_str);
	printf("Usage: wavemon [ -dhlrv ] [ -i ifname ]\n\n");
	printf("  -d            Dump the current device status to stdout and exit\n");
	printf("  -h            This help screen\n");
	printf("  -i <ifname>   Use specified network interface (default: auto)\n");
	printf("  -l            Use linear scales in favour of algorithmic ones\n");
	printf("  -r            Generate random levels (for testing purposes)\n");
	printf("  -v            Print version number and exit\n\n");
	exit(0);
}

void version_exit()
{
	printf("%s\n%s\n\n", version_str, license_str);
	exit(0);
}

void init_conf_items(struct wavemon_conf *conf)
{
	struct conf_item *item;
	int	if_list = iw_getif();
	int 	s = sizeof(struct conf_item);

	if (!ll_size(if_list)) fatal_error("no wireless interfaces found!");

	conf_items = ll_create();

	item = calloc(1, s);
	item->name = strdup("Interface");
	item->type = t_sep;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Interface");
	item->cfname	= strdup("interface");
	item->type	= t_listval;
	item->v.s	= strdup(conf->ifname);
	item->max	= 10;
	item->list	= if_list;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Statistics updates");
	item->cfname	= strdup("stat_updates");
	item->type	= t_int;
	item->v.i	= &conf->stat_iv;
	item->min	= 10;
	item->max	= 4000;
	item->inc	= 10;
	item->unit	= strdup("ms");
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Histogram update cycles");
	item->cfname	= strdup("lhist_slot_size");
	item->type	= t_int;
	item->v.i	= &conf->slotsize;
	item->min	= 1;
	item->max	= 64;
	item->inc	= 1;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Dynamic info updates");
	item->cfname	= strdup("info_updates");
	item->type	= t_int;
	item->v.i	= &conf->info_iv;
	item->min	= 1;
	item->max	= 60;
	item->inc	= 1;
	item->unit = strdup("s");
	ll_push(conf_items, "*", item);

	/* level scale items */
	item = calloc(1, s);
	item->type = t_sep;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name = strdup("Level scales");
	item->type = t_sep;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Override scale autodetect");
	item->cfname	= strdup("override_auto_scale");
	item->type	= t_switch;
	item->v.b	= &conf->override_bounds;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Minimum signal level");
	item->cfname	= strdup("min_signal_level");
	item->type	= t_int;
	item->v.i	= &conf->sig_min;
	item->min	= -120;
	item->max	= -60;
	item->inc	= 1;
	item->unit	= strdup("dBm");
	item->dep	= &conf->override_bounds;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Maximum signal level");
	item->cfname	= strdup("max_signal_level");
	item->type	= t_int;
	item->v.i	= &conf->sig_max;
	item->min	= -59;
	item->max	= 120;
	item->inc	= 1;
	item->unit	= strdup("dBm");
	item->dep	= &conf->override_bounds;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Minimum noise level");
	item->cfname	= strdup("min_noise_level");
	item->type	= t_int;
	item->v.i	= &conf->noise_min;
	item->min	= -120;
	item->max	= -60;
	item->inc	= 1;
	item->unit	= strdup("dBm");
	item->dep	= &conf->override_bounds;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Maximum noise level");
	item->cfname	= strdup("max_noise_level");
	item->type	= t_int;
	item->v.i	= &conf->noise_max;
	item->min	= -60;
	item->max	= 120;
	item->inc	= 1;
	item->unit	= strdup("dBm");
	item->dep	= &conf->override_bounds;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Linear level scales");
	item->cfname	= strdup("linear_scale");
	item->type	= t_switch;
	item->v.b	= &conf->linear;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Random signals");
	item->cfname	= strdup("random");
	item->type	= t_switch;
	item->v.b	= &conf->random;
	ll_push(conf_items, "*", item);

	/* thresholds */
	item = calloc(1, s);
	item->name	= strdup("Low threshold action");
	item->cfname	= strdup("lo_threshold_action");
	item->type	= t_list;
	item->v.b	= &conf->lthreshold_action;
	item->list	= ll_create();
	ll_push(item->list, "s", "Disabled");
	ll_push(item->list, "s", "Beep");
	ll_push(item->list, "s", "Flash");
	ll_push(item->list, "s", "Beep+Flash");
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Low threshold");
	item->cfname	= strdup("lo_threshold");
	item->type	= t_int;
	item->v.i	= &conf->lthreshold;
	item->min	= -120;
	item->max	= -60;
	item->inc	= 1;
	item->unit	= strdup("dBm");
	item->dep	= &conf->lthreshold_action;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("High threshold action");
	item->cfname	= strdup("hi_threshold_action");
	item->type	= t_list;
	item->v.b	= &conf->hthreshold_action;
	item->list	= ll_create();
	ll_push(item->list, "s", "Disabled");
	ll_push(item->list, "s", "Beep");
	ll_push(item->list, "s", "Flash");
	ll_push(item->list, "s", "Beep+Flash");
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("High threshold");
	item->cfname	= strdup("hi_threshold");
	item->type	= t_int;
	item->v.i	= &conf->hthreshold;
	item->min	= -59;
	item->max	= 120;
	item->inc	= 1;
	item->unit	= strdup("dBm");
	item->dep	= &conf->hthreshold_action;
	ll_push(conf_items, "*", item);

	/* start-up items */
	item = calloc(1, s);
	item->type = t_sep;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name = strdup("Startup");
	item->type = t_sep;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Startup screen");
	item->cfname	= strdup("startup_screen");
	item->type	= t_list;
	item->v.b	= &conf->startup_scr;
	item->list	= ll_create();
	ll_push(item->list, "s", "Info");
	ll_push(item->list, "s", "Histogram");
	ll_push(item->list, "s", "Access points");
	ll_push(conf_items, "*", item);

	/* functions */
	item = calloc(1, s);
	item->type = t_sep;
	ll_push(conf_items, "*", item);

	item = calloc(1, s);
	item->name	= strdup("Save configuration");
	item->type	= t_func;
	item->v.fp	= write_cf;
	ll_push(conf_items, "*", item);
}

void getargs(struct wavemon_conf *conf, int argc, char *argv[])
{
	int arg, tmp;

	while ((arg = getopt(argc, argv, "dhi:lrv")) >= 0)
		switch (arg) {
			case 'd':
				conf->dump = true;
				break;
			case 'h':
				help_exit();
				break;
			case 'i':
				if ((tmp = ll_scan(iw_getif(), "S", optarg)) >= 0)
					strncpy(conf->ifname, ll_get(iw_getif(), tmp), sizeof(conf->ifname));
				else
					fatal_error("no wireless extensions found on device %s", optarg);
				break;
			case 'l':
				conf->linear = true;
				break;
			case 'r':
				conf->random = true;
				break;
			case 'v':
				version_exit();
				break;
			default:
				/* bad argument. bad bad */
				exit(-1);
		}
}

void read_cf(struct wavemon_conf *conf)
{
	char	tmp[0x100], lv[0x20], rv[0x20], *cfname;
	char	*lp;
	FILE	*fd;
	int 	lnum = 0;
	char	found;
	struct conf_item *ci = NULL;
	struct passwd *pw = getpwuid(getuid());
	int	v_int;
	char	*conv_err;

	cfname = malloc(strlen(pw->pw_dir) + strlen(CFNAME) + 3);
	strcpy(cfname, pw->pw_dir);
	strcat(cfname, "/"); strcat(cfname, CFNAME);

	if ((fd = fopen(cfname, "r"))) {
		while (fgets(tmp, 0x100, fd)) {
			found = 0;
			++lnum;
			lp = tmp + strspn(tmp, " ");
			if (*lp != '#' && *lp != '\n') {
				if (strcspn(lp, " =") > sizeof(lv)) {
					fclose(fd);
					fatal_error("parse error in %s, line %d: identifier too long", cfname, lnum);
				}
				strncpy(lv, lp, strcspn(lp, " ="));
				lv[strcspn(lp, " =")] = '\0';

				ll_reset(conf_items);
				while (!found && (ci = ll_getall(conf_items)))
					if (ci->type != t_sep && ci->type != t_func && !strcasecmp(ci->cfname, lv))
						found = 1;
				if (!found) {
					fclose(fd);
					fatal_error("parse error in %s, line %d: unknown identifier '%s'", cfname, lnum, lv);
				}

				lp += strlen(lv);
				lp += strspn(lp, " ");
				if (*lp++ != '=') {
					fclose(fd);
					fatal_error("parse error in %s, line %d: missing '=' operator in assignment", cfname, lnum);
				}
				lp += strspn(lp, " ");

				if (strcspn(lp, " \n") > sizeof(rv)) {
					fclose(fd);
					fatal_error("parse error in %s, line %d: argument too long", cfname, lnum);
				}
				if (*lp == '\n') {
					fclose(fd);
					fatal_error("parse error in %s, line %d: argument expected", cfname, lnum);
				}
				strncpy(rv, lp, strcspn(lp, " \n"));
				rv[strcspn(lp, " \n")] = '\0';

				switch (ci->type) {
					case t_int:
						v_int = strtol(rv, &conv_err, 10);
						if (*conv_err == '\0') {
							if (v_int > ci->max) {
								fclose(fd);
								fatal_error("parse error in %s, line %d: value exceeds maximum of %d",
									    cfname, lnum, (int)ci->max);
							} else if (v_int < ci->min) {
								fclose(fd);
								fatal_error("parse error in %s, line %d: value is below minimum of %d",
									    cfname, lnum, (int)ci->min);
							} else {
								*ci->v.i = v_int;
							}
						} else {
							fclose(fd);
							fatal_error("parse error in %s, line %d: integer value expected, '%s' found instead",
								    cfname, lnum, rv);
						}
						break;
					case t_string:
						if (strlen(rv) <= ci->max) {
							ci->v.s = strdup(rv);
						} else {
							fclose(fd);
							fatal_error("parse error in %s, line %d: argument too long (max %d chars)",
								    cfname, lnum, ci->max);
						}
						break;
					case t_switch:
						if (!strcasecmp(rv, "on") || !strcasecmp(rv, "enabled")
						    || !strcasecmp(rv, "yes") || !strcasecmp(rv, "1")) {
							*(ci->v.b) = 1;
						} else if (!strcasecmp(rv, "off") || !strcasecmp(rv, "disabled")
							|| !strcasecmp(rv, "no") || !strcasecmp(rv, "0")) {
							*(ci->v.b) = 0;
						} else {
							fclose(fd);
							fatal_error("parse error in %s, line %d: boolean expected, '%s' found instead",
								    cfname, lnum, rv);
						}
						break;
					case t_list:
						if ((v_int = ll_scan(ci->list, "S", rv)) >= 0) {
							*ci->v.b = v_int;
						} else {
							fclose(fd);
							fatal_error("parse error in %s, line %d: '%s' is not a valid argument here",
								    cfname, lnum, rv);
						}
						break;
					case t_listval:
						if ((v_int = ll_scan(ci->list, "S", rv)) >= 0) {
							ci->v.s = strdup(ll_get(ci->list, v_int));
						} else {
							fclose(fd);
							fatal_error("parse error in %s, line %d: '%s' is not a valid argument here",
								    cfname, lnum, rv);
						}
					case t_sep:	/* These two cases are missing from the enum, they are not handled */
					case t_func:	/* To pacify gcc -Wall, fall through here */
						break;

				}
			}
		}
		fclose(fd);
	} else if (errno != ENOENT) {
		fatal_error("cannot open %s: %s", cfname, strerror(errno));
	}
}

void write_cf()
{
	struct passwd *pw = getpwuid(getuid());
	struct conf_item *ci = NULL;
	char	tmp[0x100], rv[0x40], *cfname;
	char	*lp, *cp;
	FILE	*fd;
	int	cfld;
	int	add;
	int	i;

	cfname = malloc(strlen(pw->pw_dir) + strlen(CFNAME) + 3);
	strcpy(cfname, pw->pw_dir);
	strcat(cfname, "/"); strcat(cfname, CFNAME);

	cfld = ll_create();
	if ((fd = fopen(cfname, "r"))) {
		while (fgets(tmp, 0x100, fd))
			ll_push(cfld, "s", tmp);
		fclose(fd);
	}

	ll_reset(conf_items);
	while ((ci = ll_getall(conf_items))) {
		if (ci->type != t_sep && ci->type != t_func &&
		    (!ci->dep || (ci->dep && *ci->dep))) {
			switch (ci->type) {
				case t_int:
					sprintf(rv, "%d", *ci->v.i);
					break;
				case t_string:	/* fall through */
				case t_listval:
					strcpy(rv, ci->v.s);
					break;
				case t_switch:
					sprintf(rv, "%s", *ci->v.b ? "on" : "off");
					break;
				case t_list:
					sprintf(rv, "%s", (char *)ll_get(ci->list, *ci->v.b));
					str_tolower(rv);
					break;
				/* Fall through, the rest are dummy statements to pacify gcc -Wall */
				case t_sep:
				case t_func:
					break;
			}

			add = 1;

			for (i = 0; i < ll_size(cfld); i++) {
				lp = ll_get(cfld, i);
				cp = lp += strspn(lp, " ");
				if (!strncasecmp(cp, ci->cfname, strcspn(cp, " ="))
					&& strlen(ci->cfname) == strcspn(cp, " =")) {
					add = 0;
					cp += strcspn(cp, "=") + 1;
					cp += strspn(cp, " ");
					strncpy(tmp, cp, strcspn(cp, " #\n"));
					if (strcasecmp(tmp, rv)) {
						strncpy(tmp, lp, strcspn(lp, " ="));
						tmp[strcspn(lp, " =")] = '\0';
						strcat(tmp, " = ");
						strcat(tmp, rv);
						strcat(tmp, "\n");
						ll_replace(cfld, i, "s", tmp);
					}
				}
			}

			if (add) {
				strcpy(tmp, ci->cfname);
				strcat(tmp, " = ");
				strcat(tmp, rv);
				strcat(tmp, "\n");
				ll_push(cfld, "s", tmp);
			}
		}
	}

	if ((fd = fopen(cfname, "w")) < 0)
		fatal_error("cannot open %s", cfname);
	ll_reset(cfld);
	while ((lp = ll_getall(cfld)))
		fputs(lp, fd);
	fclose(fd);

	ll_destroy(cfld);
	free(cfname);
}

void set_defaults(struct wavemon_conf *conf)
{
	strncpy(conf->ifname, ll_get(iw_getif(), 0), sizeof(conf->ifname));

	conf->stat_iv		= 100;
	conf->info_iv		= 10;
	conf->slotsize		= 4;

	conf->override_bounds	= false;
	conf->dump		= false;
	conf->random		= false;
	conf->linear		= false;

	conf->sig_min		= -102;
	conf->sig_max		= 10;
	conf->noise_min		= -102;
	conf->noise_max		= 10;


	conf->lthreshold_action	= 0;	/* Disabled, Beep, Flash, Beep+Flash */
	conf->lthreshold	= -80;
	conf->hthreshold_action	= 0;	/* Disabled, Beep, Flash, Beep+Flash */
	conf->hthreshold	= -10;

	conf->startup_scr	= 0;
}

void dealloc_on_exit()
{
	struct conf_item *ci;

	if (!conf_items)
		return;

	while ((ci = ll_getall(conf_items))) {
		if (ci->type == t_list)
			ll_destroy(ci->list);
	}
	ll_destroy(conf_items);
}

void getconf(struct wavemon_conf *conf, int argc, char *argv[])
{
	set_defaults(conf);
	init_conf_items(conf);
	read_cf(conf);
	getargs(conf, argc, argv);
}
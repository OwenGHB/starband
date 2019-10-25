/* File: init2.c */


/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

#include "init.h"


/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * The "init1.c" file is used only to parse the ascii template files,
 * to create the binary image files.  If you include the binary image
 * files instead of the ascii template files, then you can undefine
 * "ALLOW_TEMPLATES", saving about 20K by removing "init1.c".  Note
 * that the binary image files are extremely system dependant.
 */



/*
 * Find the default paths to all of our important sub-directories.
 *
 * The purpose of each sub-directory is described in "variable.c".
 *
 * All of the sub-directories should, by default, be located inside
 * the main "lib" directory, whose location is very system dependant.
 *
 * This function takes a writable buffer, initially containing the
 * "path" to the "lib" directory, for example, "/pkg/lib/angband/",
 * or a system dependant string, for example, ":lib:".  The buffer
 * must be large enough to contain at least 32 more characters.
 *
 * Various command line options may allow some of the important
 * directories to be changed to user-specified directories, most
 * importantly, the "info" and "user" and "save" directories,
 * but this is done after this function, see "main.c".
 *
 * In general, the initial path should end in the appropriate "PATH_SEP"
 * string.  All of the "sub-directory" paths (created below or supplied
 * by the user) will NOT end in the "PATH_SEP" string, see the special
 * "path_build()" function in "util.c" for more information.
 *
 * Mega-Hack -- support fat raw files under NEXTSTEP, using special
 * "suffixed" directories for the "ANGBAND_DIR_DATA" directory, but
 * requiring the directories to be created by hand by the user.
 *
 * Hack -- first we free all the strings, since this is known
 * to succeed even if the strings have not been allocated yet,
 * as long as the variables start out as "NULL".  This allows
 * this function to be called multiple times, for example, to
 * try several base "path" values until a good one is found.
 */
void init_file_paths(char *path)
{
	char *tail;

#ifdef SET_UID
	char buf[1024];
#endif /* SET_UID */

	/*** Free everything ***/

	/* Free the main path */
	string_free(ANGBAND_DIR);

	/* Free the sub-paths */
	string_free(ANGBAND_DIR_APEX);
	string_free(ANGBAND_DIR_BONE);
	string_free(ANGBAND_DIR_DATA);
	string_free(ANGBAND_DIR_EDIT);
	string_free(ANGBAND_DIR_FILE);
	string_free(ANGBAND_DIR_HELP);
	string_free(ANGBAND_DIR_INFO);
	string_free(ANGBAND_DIR_SAVE);
	string_free(ANGBAND_DIR_PREF);
	string_free(ANGBAND_DIR_USER);
	string_free(ANGBAND_DIR_XTRA);


	/*** Prepare the "path" ***/

	/* Hack -- save the main directory */
	ANGBAND_DIR = string_make(path);

	/* Prepare to append to the Base Path */
	tail = path + strlen(path);


#ifdef VM


	/*** Use "flat" paths with VM/ESA ***/

	/* Use "blank" path names */
	ANGBAND_DIR_APEX = string_make("");
	ANGBAND_DIR_BONE = string_make("");
	ANGBAND_DIR_DATA = string_make("");
	ANGBAND_DIR_EDIT = string_make("");
	ANGBAND_DIR_FILE = string_make("");
	ANGBAND_DIR_HELP = string_make("");
	ANGBAND_DIR_INFO = string_make("");
	ANGBAND_DIR_SAVE = string_make("");
	ANGBAND_DIR_PREF = string_make("");
	ANGBAND_DIR_USER = string_make("");
	ANGBAND_DIR_XTRA = string_make("");


#else /* VM */


	/*** Build the sub-directory names ***/

	/* Build a path name */
	strcpy(tail, "apex");
	ANGBAND_DIR_APEX = string_make(path);

	/* Build a path name */
	strcpy(tail, "bone");
	ANGBAND_DIR_BONE = string_make(path);

	/* Build a path name */
	strcpy(tail, "data");
	ANGBAND_DIR_DATA = string_make(path);

	/* Build a path name */
	strcpy(tail, "edit");
	ANGBAND_DIR_EDIT = string_make(path);

	/* Build a path name */
	strcpy(tail, "file");
	ANGBAND_DIR_FILE = string_make(path);

	/* Build a path name */
	strcpy(tail, "help");
	ANGBAND_DIR_HELP = string_make(path);

	/* Build a path name */
	strcpy(tail, "info");
	ANGBAND_DIR_INFO = string_make(path);

	/* Build a path name */
	strcpy(tail, "save");
	ANGBAND_DIR_SAVE = string_make(path);

	/* Build a path name */
	strcpy(tail, "pref");
	ANGBAND_DIR_PREF = string_make(path);

#ifdef PRIVATE_USER_PATH

	/* Build the path to the user specific directory */
	path_build(buf, 1024, PRIVATE_USER_PATH, VERSION_NAME);

	/* Build a relative path name */
	ANGBAND_DIR_USER = string_make(buf);

#else /* PRIVATE_USER_PATH */

	/* Build a path name */
	strcpy(tail, "user");
	ANGBAND_DIR_USER = string_make(path);

#endif /* PRIVATE_USER_PATH */

	/* Build a path name */
	strcpy(tail, "xtra");
	ANGBAND_DIR_XTRA = string_make(path);

#endif /* VM */


#ifdef NeXT

	/* Allow "fat binary" usage with NeXT */
	if (TRUE)
	{
		cptr next = NULL;

# if defined(m68k)
		next = "m68k";
# endif

# if defined(i386)
		next = "i386";
# endif

# if defined(sparc)
		next = "sparc";
# endif

# if defined(hppa)
		next = "hppa";
# endif

		/* Use special directory */
		if (next)
		{
			/* Forget the old path name */
			string_free(ANGBAND_DIR_DATA);

			/* Build a new path name */
			sprintf(tail, "data-%s", next);
			ANGBAND_DIR_DATA = string_make(path);
		}
	}

#endif /* NeXT */

}



#ifdef ALLOW_TEMPLATES


/*
 * Hack -- help give useful error messages
 */
int error_idx;
int error_line;


/*
 * Standard error message text
 */
static cptr err_str[PARSE_ERROR_MAX] =
{
	NULL,
	"parse error",
	"obsolete file",
	"missing record header",
	"non-sequential records",
	"invalid flag specification",
	"undefined directive",
	"out of memory",
	"value out of bounds",
	"too few arguments",
	"too many arguments",
	"too many allocation entries",
	"invalid spell frequency",
	"invalid number of items (0-99)",
	"too many entries",
	"non-sequential quest levels",
};


#endif /* ALLOW_TEMPLATES */


/*
 * File headers
 */
header z_head;
header v_head;
header f_head;
header k_head;
header a_head;
header e_head;
header r_head;
header p_head;
header c_head;
header h_head;
header b_head;
header g_head;
header q_head;



/*** Initialize from binary image files ***/


/*
 * Initialize a "*_info" array, by parsing a binary "image" file
 */
static errr init_info_raw(int fd, header *head)
{
	header test;


	/* Read and verify the header */
	if (fd_read(fd, (char*)(&test), sizeof(header)) ||
	    (test.v_major != head->v_major) ||
	    (test.v_minor != head->v_minor) ||
	    (test.v_patch != head->v_patch) ||
	    (test.v_extra != head->v_extra) ||
	    (test.info_num != head->info_num) ||
	    (test.info_len != head->info_len) ||
	    (test.head_size != head->head_size) ||
	    (test.info_size != head->info_size))
	{
		/* Error */
		return (-1);
	}


	/* Accept the header */
	COPY(head, &test, header);


	/* Allocate the "*_info" array */
	C_MAKE(head->info_ptr, head->info_size, char);

	/* Read the "*_info" array */
	fd_read(fd, head->info_ptr, head->info_size);

	if (head->name_size)
	{
		/* Allocate the "*_name" array */
		C_MAKE(head->name_ptr, head->name_size, char);

		/* Read the "*_name" array */
		fd_read(fd, head->name_ptr, head->name_size);
	}

	if (head->text_size)
	{
		/* Allocate the "*_text" array */
		C_MAKE(head->text_ptr, head->text_size, char);

		/* Read the "*_text" array */
		fd_read(fd, head->text_ptr, head->text_size);
	}

	/* Success */
	return (0);
}


/*
 * Initialize the header of an *_info.raw file.
 */
static void init_header(header *head, int num, int len)
{
	/* Save the "version" */
	head->v_major = VERSION_MAJOR;
	head->v_minor = VERSION_MINOR;
	head->v_patch = VERSION_PATCH;
	head->v_extra = VERSION_EXTRA;

	/* Save the "record" information */
	head->info_num = num;
	head->info_len = len;

	/* Save the size of "*_head" and "*_info" */
	head->head_size = sizeof(header);
	head->info_size = head->info_num * head->info_len;
}


#ifdef ALLOW_TEMPLATES

/*
 * Display a parser error message.
 */
static void display_parse_error(cptr filename, errr err, cptr buf)
{
	cptr oops;

	/* Error string */
	oops = (((err > 0) && (err < PARSE_ERROR_MAX)) ? err_str[err] : "unknown");

	/* Oops */
	msg_format("Error at line %d of '%s'.", error_line, filename);
	msg_format("Record %d contains a '%s' error.", error_idx, oops);
	msg_format("Parsing '%s'.", buf);
	message_flush();

	/* Quit */
	quit_fmt("Error in '%s.txt' file.", filename);
}

#endif /* ALLOW_TEMPLATES */


/*
 * Initialize a "*_info" array
 *
 * Note that we let each entry have a unique "name" and "text" string,
 * even if the string happens to be empty (everyone has a unique '\0').
 */
static errr init_info(cptr filename, header *head,
                      void **info, char **name, char **text)
{
	int fd;

	errr err = 1;

	FILE *fp;

	/* General buffer */
	char buf[1024];


#ifdef ALLOW_TEMPLATES

	/*** Load the binary image file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_DATA, format("%s.raw", filename));

	/* Attempt to open the "raw" file */
	fd = fd_open(buf, O_RDONLY);

	/* Process existing "raw" file */
	if (fd >= 0)
	{
#ifdef CHECK_MODIFICATION_TIME

		err = check_modification_date(fd, format("%s.txt", filename));

#endif /* CHECK_MODIFICATION_TIME */

		/* Attempt to parse the "raw" file */
		if (!err)
			err = init_info_raw(fd, head);

		/* Close it */
		fd_close(fd);
	}

	/* Do we have to parse the *.txt file? */
	if (err)
	{
		/*** Make the fake arrays ***/

		/* Allocate the "*_info" array */
		C_MAKE(head->info_ptr, head->info_size, char);

		/* Hack -- make "fake" arrays */
		if (name)
			C_MAKE(head->name_ptr, z_info->fake_name_size, char);

		if (text)
			C_MAKE(head->text_ptr, z_info->fake_text_size, char);

		if (info) (*info) = head->info_ptr;
		if (name) (*name) = head->name_ptr;
		if (text) (*text) = head->text_ptr;

		/*** Load the ascii template file ***/

		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_EDIT, format("%s.txt", filename));

		/* Open the file */
		fp = my_fopen(buf, "r");

		/* Parse it */
		if (!fp) quit(format("Cannot open '%s.txt' file.", filename));

		/* Parse the file */
		err = init_info_txt(fp, buf, head, head->parse_info_txt);

		/* Close it */
		my_fclose(fp);

		/* Errors */
		if (err) display_parse_error(filename, err, buf);


		/*** Dump the binary image file ***/

		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_DATA, format("%s.raw", filename));


		/* Attempt to open the file */
		fd = fd_open(buf, O_RDONLY);

		/* Failure */
		if (fd < 0)
		{
			int mode = 0644;

			/* Grab permissions */
			safe_setuid_grab();

			/* Create a new file */
			fd = fd_make(buf, mode);

			/* Drop permissions */
			safe_setuid_drop();

			/* Failure */
			if (fd < 0)
			{
				char why[1024];

				/* Message */
				sprintf(why, "Cannot create the '%s' file!", buf);

				/* Crash and burn */
				quit(why);
			}
		}

		/* Close it */
		fd_close(fd);

		/* Grab permissions */
		safe_setuid_grab();

		/* Attempt to create the raw file */
		fd = fd_open(buf, O_WRONLY);

		/* Drop permissions */
		safe_setuid_drop();

		/* Dump to the file */
		if (fd >= 0)
		{
			/* Dump it */
			fd_write(fd, (cptr)head, head->head_size);

			/* Dump the "*_info" array */
			fd_write(fd, head->info_ptr, head->info_size);

			/* Dump the "*_name" array */
			fd_write(fd, head->name_ptr, head->name_size);

			/* Dump the "*_text" array */
			fd_write(fd, head->text_ptr, head->text_size);

			/* Close */
			fd_close(fd);
		}


		/*** Kill the fake arrays ***/

		/* Free the "*_info" array */
		C_KILL(head->info_ptr, head->info_size, char);

		/* Hack -- Free the "fake" arrays */
		if (name)
			C_KILL(head->name_ptr, z_info->fake_name_size, char);

		if (text)
			C_KILL(head->text_ptr, z_info->fake_text_size, char);

#endif /* ALLOW_TEMPLATES */


		/*** Load the binary image file ***/

		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_DATA, format("%s.raw", filename));

		/* Attempt to open the "raw" file */
		fd = fd_open(buf, O_RDONLY);

		/* Process existing "raw" file */
		if (fd < 0) quit(format("Cannot load '%s.raw' file.", filename));

		/* Attempt to parse the "raw" file */
		err = init_info_raw(fd, head);

		/* Close it */
		fd_close(fd);

		/* Error */
		if (err) quit(format("Cannot parse '%s.raw' file.", filename));

#ifdef ALLOW_TEMPLATES
	}
#endif /* ALLOW_TEMPLATES */

	if (info) (*info) = head->info_ptr;
	if (name) (*name) = head->name_ptr;
	if (text) (*text) = head->text_ptr;

	/* Success */
	return (0);
}


/*
 * Free the allocated memory for the info-, name-, and text- arrays.
 */
static errr free_info(header *head)
{
	if (head->info_ptr)
		C_FREE(head->info_ptr, head->info_size, char);

	if (head->name_ptr)
		C_FREE(head->name_ptr, head->name_size, char);

	if (head->text_ptr)
		C_FREE(head->text_ptr, head->text_size, char);

	/* Success */
	return (0);
}


/*
 * Initialize the "z_info" array
 */
static errr init_z_info(void)
{
	/* Init the header */
	init_header(&z_head, 1, sizeof(maxima));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	z_head.parse_info_txt = parse_z_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("limits", &z_head,
	                 (void*)&z_info, NULL, NULL);
}


/*
 * Initialize the "f_info" array
 */
static errr init_f_info(void)
{
	/* Init the header */
	init_header(&f_head, z_info->f_max, sizeof(feature_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	f_head.parse_info_txt = parse_f_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("terrain", &f_head,
	                 (void*)&f_info, (void*)&f_name, (void*)&f_text);
}



/*
 * Initialize the "k_info" array
 */
static errr init_k_info(void)
{
	/* Init the header */
	init_header(&k_head, z_info->k_max, sizeof(object_kind));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	k_head.parse_info_txt = parse_k_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("object", &k_head,
	                 (void*)&k_info, (void*)&k_name, (void*)&k_text);
}



/*
 * Initialize the "a_info" array
 */
static errr init_a_info(void)
{
	/* Init the header */
	init_header(&a_head, z_info->a_max, sizeof(artifact_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	a_head.parse_info_txt = parse_a_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("artifact", &a_head,
	                 (void*)&a_info, (void*)&a_name, (void*)&a_text);
}



/*
 * Initialize the "e_info" array
 */
static errr init_e_info(void)
{
	/* Init the header */
	init_header(&e_head, z_info->e_max, sizeof(ego_item_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	e_head.parse_info_txt = parse_e_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("ego_item", &e_head,
	                 (void*)&e_info, (void*)&e_name, (void*)&e_text);
}



/*
 * Initialize the "r_info" array
 */
static errr init_r_info(void)
{
	/* Init the header */
	init_header(&r_head, z_info->r_max, sizeof(monster_race));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	r_head.parse_info_txt = parse_r_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("monster", &r_head,
	                 (void*)&r_info, (void*)&r_name, (void*)&r_text);
}



/*
 * Initialize the "v_info" array
 */
static errr init_v_info(void)
{
	/* Init the header */
	init_header(&v_head, z_info->v_max, sizeof(vault_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	v_head.parse_info_txt = parse_v_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("vault", &v_head,
	                 (void*)&v_info, (void*)&v_name, (void*)&v_text);
}


/*
 * Initialize the "p_info" array
 */
static errr init_p_info(void)
{
	/* Init the header */
	init_header(&p_head, z_info->p_max, sizeof(player_race));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	p_head.parse_info_txt = parse_p_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("p_race", &p_head,
	                 (void*)&p_info, (void*)&p_name, (void*)&p_text);
}


/*
 * Initialize the "c_info" array
 */
static errr init_c_info(void)
{
	/* Init the header */
	init_header(&c_head, z_info->c_max, sizeof(player_class));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	c_head.parse_info_txt = parse_c_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("p_class", &c_head,
	                 (void*)&c_info, (void*)&c_name, (void*)&c_text);
}



/*
 * Initialize the "h_info" array
 */
static errr init_h_info(void)
{
	/* Init the header */
	init_header(&h_head, z_info->h_max, sizeof(hist_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	h_head.parse_info_txt = parse_h_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("p_hist", &h_head,
	                 (void*)&h_info, NULL, (void*)&h_text);
}



/*
 * Initialize the "b_info" array
 */
static errr init_b_info(void)
{
	/* Init the header */
	init_header(&b_head, (u16b)(MAX_STORES * z_info->b_max), sizeof(owner_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	b_head.parse_info_txt = parse_b_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("shop_own", &b_head,
	                 (void*)&b_info, (void*)&b_name, (void*)&b_text);
}



/*
 * Initialize the "g_info" array
 */
static errr init_g_info(void)
{
	/* Init the header */
	init_header(&g_head, (u16b)(z_info->p_max * z_info->p_max), sizeof(byte));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	g_head.parse_info_txt = parse_g_info;

#endif /* ALLOW_TEMPLATES */

	return init_info("cost_adj", &g_head,
	                 (void*)&g_info, (void*)&g_name, (void*)&g_text);
}



/*
 * Initialize the "q_info" array
 */
static errr init_q_info(void)
{
	errr err;

	/* Init the header */
	init_header(&q_head, z_info->q_max, sizeof(quest_type));

#ifdef ALLOW_TEMPLATES

	/* Save a pointer to the parsing function */
	q_head.parse_info_txt = parse_q_info;

#endif /* ALLOW_TEMPLATES */

    return init_info("quest", &q_head,
                       (void*)&q_info, (void*)&q_name, NULL);
}




/*** Initialize others ***/



/*
 * Hack -- Objects sold in the stores -- by tval/sval pair.
 */
static const byte store_table[MAX_STORES-4][STORE_CHOICES][2] =
{
	{
		/* General Store */

		{ TV_FOOD, SV_FOOD_MEAT_PIE },
		{ TV_FOOD, SV_FOOD_MEAT_PIE },
		{ TV_FOOD, SV_FOOD_MEAT_PIE },
		{ TV_FOOD, SV_FOOD_TURNIPS },
		{ TV_FOOD, SV_FOOD_CHEESE },
		{ TV_FOOD, SV_FOOD_ONION },
		{ TV_FOOD, SV_FOOD_POTATO },
		{ TV_FOOD, SV_FOOD_CRUMPET },

		{ TV_FOOD, SV_FOOD_MUTTON },
		{ TV_FOOD, SV_FOOD_MUTTON },
		{ TV_FOOD, SV_FOOD_PINT_OF_WINE },
		{ TV_FOOD, SV_FOOD_PINT_ERDE_WINE },
		{ TV_LITE, SV_LITE_CANDLE_TALLOW },
		{ TV_LITE, SV_LITE_CANDLE_WAX },
		{ TV_LITE, SV_LITE_CANDLE_WAX },
		{ TV_LITE, SV_LITE_TORCH },

		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_LITE, SV_LITE_TORCH },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },
		{ TV_FLASK, 0 },

		{ TV_FLASK, 0 },
		{ TV_SPIKE, 0 },
		{ TV_SPIKE, 0 },
		{ TV_AMMO, SV_AMMO_NORMAL },
		{ TV_BULLET, SV_AMMO_NORMAL },
		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_DIGGING, SV_SHOVEL },
		{ TV_DIGGING, SV_PICK }
	},

	{
		/* Clothing Store */

		{ TV_BOOTS, SV_POINTED_TOE_DANDIES },
		{ TV_BOOTS, SV_ANKLE_BOOTS },
		{ TV_BOOTS, SV_SQUARE_TOE_DANDIES },
		{ TV_HELM, SV_HARD_LEATHER_CAP },
		{ TV_HELM, SV_DEERSTALKER },
		{ TV_HELM, SV_BOWLER },
		{ TV_HELM, SV_WOOLEN_BEANIE },
		{ TV_HELM, SV_TOP_HAT },

		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_SOFT_ARMOR, SV_LEATHER_WAISTCOAT },
		{ TV_SOFT_ARMOR, SV_LEATHER_WAISTCOAT },
		{ TV_SOFT_ARMOR, SV_FANCY_SILK_VEST },
		{ TV_SOFT_ARMOR, SV_FANCY_SILK_VEST },
		{ TV_SOFT_ARMOR, SV_HEAVY_JACKET },
		{ TV_SOFT_ARMOR, SV_HEAVY_TRENCHCOAT },

		{ TV_SOFT_ARMOR, SV_TWEED_JACKET },
		{ TV_HARD_ARMOR, SV_OFFICER_UNIFORM },
		{ TV_HARD_ARMOR, SV_HUNTER_UNIFORM },
		{ TV_HARD_ARMOR, SV_PEA_JACKET },
		{ TV_HARD_ARMOR, SV_TWEED_SUIT },
		{ TV_HARD_ARMOR, SV_PROFESSIONAL },
		{ TV_HARD_ARMOR, SV_CLAY_WORSTED },
		{ TV_HARD_ARMOR, SV_VICUNA_CHEVIOT },

		{ TV_HARD_ARMOR, SV_PRINCE_ALBERT },
		{ TV_CLOAK, SV_CLOAK },
		{ TV_CLOAK, SV_CLOAK },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_LEATHER_GLOVES },
		{ TV_GLOVES, SV_SET_OF_SILK_GLOVES },
		{ TV_LEG, SV_PANTS },
		{ TV_LEG, SV_BREECHES }
	},

	{
		/* Gun Store */

		{ TV_GUN, SV_DERRINGER },
		{ TV_GUN, SV_DERRINGER },
		{ TV_GUN, SV_32_REVOLVER },
		{ TV_GUN, SV_38_REVOLVER },
		{ TV_GUN, SV_22_BOLT_ACTION },
		{ TV_GUN, SV_30_LEVER_ACTION },
		{ TV_GUN, SV_30_LEVER_ACTION },
		{ TV_GUN, SV_45_MARTINI_HENRY },
		
		{ TV_GUN, SV_COL_MORAN },
		{ TV_GUN, SV_20_GAGUE },
		{ TV_GUN, SV_16_GAGUE },
		{ TV_AMMO, SV_AMMO_LIGHT },
		{ TV_AMMO, SV_AMMO_LIGHT },
		{ TV_AMMO, SV_AMMO_NORMAL },
		{ TV_BULLET, SV_AMMO_NORMAL },

		{ TV_BULLET, SV_AMMO_NORMAL },
		{ TV_BULLET, SV_AMMO_NORMAL },
		{ TV_BULLET, SV_AMMO_NORMAL },
		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_SHOT, SV_AMMO_NORMAL },
		{ TV_SHOT, SV_AMMO_LIGHT },
		{ TV_SHOT, SV_AMMO_LIGHT },
		{ TV_HAFTED, SV_WHIP },
		{ TV_HAFTED, SV_MACE },

		{ TV_POLEARM, SV_JAVELIN },
		{ TV_POLEARM, SV_SPEAR },
		{ TV_SWORD, SV_MAIN_GAUCHE },
		{ TV_SWORD, SV_RAPIER },
		{ TV_DAGGER, SV_THROWING_KNIFE },
		{ TV_DAGGER, SV_DAGGER },
		{ TV_BLUNT, SV_CANE_WSTICK },

		{ TV_AXES, SV_BEAKED_AXE }

	},

	{
		/* Machineist Shop */

		{ TV_MECHANISM, SV_MECHANISM_STAR_IDENTIFY },
		{ TV_MECHANISM, SV_MECHANISM_LIGHT },
		{ TV_MECHANISM, SV_MECHANISM_POWER_CELL },
		{ TV_MECHANISM, SV_MECHANISM_REMOVE_CURSE },
		{ TV_MECHANISM, SV_MECHANISM_IDENTIFY },
		{ TV_MECHANISM, SV_MECHANISM_MECHA_HEALING },
		{ TV_MECHANISM, SV_MECHANISM_REMOVE_CURSE },
		{ TV_MECHANISM, SV_MECHANISM_TELEPORT },

		{ TV_MECHANISM, SV_MECHANISM_TELEPORT },
		{ TV_MECHANISM, SV_MECHANISM_PHASE_DOOR },
		{ TV_MECHANISM, SV_MECHANISM_WORD_OF_RECALL },
		{ TV_MECHANISM, SV_MECHANISM_PHASE_DOOR },
		{ TV_MECHANISM, SV_MECHANISM_WORD_OF_RECALL },
		{ TV_MECHANISM, SV_MECHANISM_DETECT_INVIS },
		{ TV_MECHANISM, SV_MECHANISM_MECHA_HEALING },
		{ TV_MECHANISM, SV_MECHANISM_LIGHT },
		
		{ TV_MECHANISM, SV_MECHANISM_MECHA_HEALING },
		{ TV_MECHANISM, SV_MECHANISM_MECHA_HEALING },
		{ TV_MECHANISM, SV_MECHANISM_WORD_OF_RECALL },
		{ TV_MECHANISM, SV_MECHANISM_WORD_OF_RECALL },
		{ TV_MECHA_HEAD, SV_MECHA_HEAD_TIN },
		{ TV_MECHA_HEAD, SV_MECHA_HEAD_TIN },
		{ TV_MECHA_ARMS, SV_MECHA_ARMS_TIN },
		{ TV_MECHA_FEET, SV_MECHA_FEET_TIN },

		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE1 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE1 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE1 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE2 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE3 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE3 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE4 },
		{ TV_MAGIC_BOOK, SV_BOOK_DEVICE4 }
	},

	{
		/* Alchemy shop */

		{ TV_TONIC, SV_TONIC_RES_MUS },
		{ TV_TONIC, SV_TONIC_RES_MUS },
		{ TV_TONIC, SV_TONIC_RES_AGI },
		{ TV_TONIC, SV_TONIC_RES_AGI },
		{ TV_TONIC, SV_TONIC_RES_VIG },
		{ TV_TONIC, SV_TONIC_RES_VIG },

		{ TV_TONIC, SV_TONIC_RES_SCH },
		{ TV_TONIC, SV_TONIC_RES_SCH },
		{ TV_TONIC, SV_TONIC_RES_EGO },
		{ TV_TONIC, SV_TONIC_RES_EGO },
		{ TV_TONIC, SV_TONIC_RES_CHR },
		{ TV_TONIC, SV_TONIC_RES_CHR },

		{ TV_TONIC, SV_TONIC_RESTORE_EXP },
		{ TV_TONIC, SV_TONIC_RESTORE_EXP },
		{ TV_TONIC, SV_TONIC_CURE_LIGHT_HP },
		{ TV_TONIC, SV_TONIC_CURE_LIGHT_HP },
		{ TV_TONIC, SV_TONIC_CURE_LIGHT_HP },
		{ TV_TONIC, SV_TONIC_CURE_LIGHT_HP },

		{ TV_TONIC, SV_TONIC_CURE_SERIOUS_HP },
		{ TV_TONIC, SV_TONIC_CURE_SERIOUS_HP },
		{ TV_TONIC, SV_TONIC_CURE_SERIOUS_HP },
		{ TV_TONIC, SV_TONIC_CURE_SERIOUS_HP },
		{ TV_TONIC, SV_TONIC_CURE_CRITICAL_HP },
		{ TV_TONIC, SV_TONIC_CURE_CRITICAL_HP },

		{ TV_TONIC, SV_TONIC_BOLDNESS },
		{ TV_TONIC, SV_TONIC_HEROISM },
		{ TV_FOOD, SV_FOOD_RESTORE_MUS },
		{ TV_FOOD, SV_FOOD_RESTORE_VIG },
		{ TV_FOOD, SV_FOOD_RESTORING },
		{ TV_FOOD, SV_FOOD_LIGHT_HEALING },
		{ TV_FOOD, SV_FOOD_CURE_SERIOUS },
		{ TV_FOOD, SV_FOOD_CURE_SERIOUS }

	},

	{
		/* Magic-User store */

		{ TV_RING, SV_RING_SEARCHING },
		{ TV_RING, SV_RING_FEATHER_FALL },
		{ TV_RING, SV_RING_PROTECTION },
		{ TV_AMULET, SV_AMULET_ADORNMENT },
		{ TV_AMULET, SV_AMULET_ADORNMENT },
		{ TV_AMULET, SV_AMULET_ADORNMENT },
		{ TV_RAY, SV_RAY_FIRE_BOLT },
		{ TV_RAY, SV_RAY_FIRE_BOLT },

		{ TV_RAY, SV_RAY_SLEEP_MONSTER },
		{ TV_RAY, SV_RAY_CONFUSE_MONSTER },
		{ TV_RAY, SV_RAY_ICE_BOLT },
		{ TV_RAY, SV_RAY_ELEC_BOLT },
		{ TV_TOOL, SV_TOOL_PHOTON_GEN },
		{ TV_TOOL, SV_TOOL_MAPPING },
		{ TV_TOOL, SV_TOOL_DETECT_TRAP },
		{ TV_TOOL, SV_TOOL_DETECT_DOOR },

		{ TV_TOOL, SV_TOOL_DETECT_GOLD },
		{ TV_TOOL, SV_TOOL_DETECT_ITEM },
		{ TV_TOOL, SV_TOOL_OBJECT_ANALYSIS },
		{ TV_TOOL, SV_TOOL_OBJECT_ANALYSIS },
		{ TV_TOOL, SV_TOOL_CURE_LIGHT_HP },
		{ TV_TOOL, SV_TOOL_CURE_LIGHT_HP },
		{ TV_TOOL, SV_TOOL_OBJECT_ANALYSIS },
		{ TV_TOOL, SV_TOOL_TELEPORT },

		{ TV_MAGIC_BOOK, SV_BOOK_MAGE1 },
		{ TV_MAGIC_BOOK, SV_BOOK_MAGE1 },
		{ TV_MAGIC_BOOK, SV_BOOK_MAGE2 },
		{ TV_MAGIC_BOOK, SV_BOOK_MAGE2 },
		{ TV_MAGIC_BOOK, SV_BOOK_PRIEST1 },
		{ TV_MAGIC_BOOK, SV_BOOK_PRIEST1 },
		{ TV_MAGIC_BOOK, SV_BOOK_PRIEST2 },
		{ TV_MAGIC_BOOK, SV_BOOK_PRIEST2 }
	}
};



/*
 * Initialize some other arrays
 */
static errr init_other(void)
{
	int i, k, n;


	/*** Prepare the various "bizarre" arrays ***/

	/* Initialize the "macro" package */
	(void)macro_init();

	/* Initialize the "quark" package */
	(void)quarks_init();

	/* Initialize the "message" package */
	(void)messages_init();

	/*** Prepare grid arrays ***/

	/* Array of grids */
	C_MAKE(view_g, VIEW_MAX, u16b);

	/* Array of grids */
	C_MAKE(temp_g, TEMP_MAX, u16b);

	/* Hack -- use some memory twice */
	temp_y = ((byte*)(temp_g)) + 0;
	temp_x = ((byte*)(temp_g)) + TEMP_MAX;


	/*** Prepare dungeon arrays ***/

	/* Padded into array */
	C_MAKE(cave_info, DUNGEON_HGT, u16b_256);

	/* Feature array */
	C_MAKE(cave_feat, DUNGEON_HGT, byte_wid);

	/* Entity arrays */
	C_MAKE(cave_o_idx, DUNGEON_HGT, s16b_wid);
	C_MAKE(cave_m_idx, DUNGEON_HGT, s16b_wid);

#ifdef MONSTER_FLOW

	/* Flow arrays */
	C_MAKE(cave_cost, DUNGEON_HGT, byte_wid);
	C_MAKE(cave_when, DUNGEON_HGT, byte_wid);

#endif /* MONSTER_FLOW */

	/*** Prepare "vinfo" array ***/

	/* Used by "update_view()" */
	(void)vinfo_init();


	/*** Prepare entity arrays ***/

	/* Objects */
	C_MAKE(o_list, z_info->o_max, object_type);

	/* Monsters */
	C_MAKE(m_list, z_info->m_max, monster_type);

	/* Effects */
	C_MAKE(x_list, z_info->x_max, effect_type);

	/*** Prepare lore array ***/

	/* Lore */
	C_MAKE(l_list, z_info->r_max, monster_lore);

	/*** Prepare the inventory ***/

	/* Allocate it */
	C_MAKE(inventory, INVEN_TOTAL, object_type);


	/*** Prepare the stores ***/

	/* Allocate the stores */
	C_MAKE(store, MAX_STORES, store_type);

	/* Fill in each store */
	for (i = 0; i < MAX_STORES; i++)
	{
		/* Get the store */
		store_type *st_ptr = &store[i];

		if (i == STORE_LIBRARY)
		{
			st_ptr->stock_size = (STORE_INVEN_MAX * 50);
		}
		else
		{
			st_ptr->stock_size = STORE_INVEN_MAX;
		}

		/* Allocate the stock */
		C_MAKE(st_ptr->stock, st_ptr->stock_size, object_type);

		/* No table for the black market or home */
		if ((i == STORE_B_MARKET) || (i == STORE_HOME)) continue;
		if ((i == STORE_LIBRARY) || (i == STORE_WETWARE)) continue;
	
		/* Assume full table */
		st_ptr->table_size = STORE_CHOICES;

		/* Allocate the stock */
		C_MAKE(st_ptr->table, st_ptr->table_size, s16b);

		/* Scan the choices */
		for (k = 0; k < STORE_CHOICES; k++)
		{
			int k_idx;

			/* Extract the tval/sval codes */
			int tv = store_table[i][k][0];
			int sv = store_table[i][k][1];

			/* Look for it */
			for (k_idx = 1; k_idx < z_info->k_max; k_idx++)
			{
				object_kind *k_ptr = &k_info[k_idx];

				/* Found a match */
				if ((k_ptr->tval == tv) && (k_ptr->sval == sv)) break;
			}

			/* Catch errors */
			if (k_idx == z_info->k_max) continue;

			/* Add that item index to the table */
			st_ptr->table[st_ptr->table_num++] = k_idx;
		}
	}



	/*** Prepare the options ***/

	/* Initialize the options */
	for (i = 0; i < OPT_MAX; i++)
	{
		/* Default value */
		op_ptr->opt[i] = option_norm[i];
	}

	/* Initialize the window flags */
	for (n = 0; n < ANGBAND_TERM_MAX; n++)
	{
		/* Assume no flags */
		op_ptr->window_flag[n] = 0L;
	}


	/*** Pre-allocate space for the "format()" buffer ***/

	/* Hack -- Just call the "format()" function */
	(void)format("%s (%s).", "Courtney Campbell", MAINTAINER);


	/* Success */
	return (0);
}



/*
 * Initialize some other arrays
 */
static errr init_alloc(void)
{
	int i;

	monster_race *r_ptr;

	ego_item_type *e_ptr;

	quest_type *q_ptr;

	alloc_entry *table;

	s16b num[MAX_DEPTH];

	s16b aux[MAX_DEPTH];


	/*** Initialize object allocation info ***/

	/* Allocate the permit_kind_table */
	C_MAKE(permit_kind_table, z_info->k_max, bool);

	/* Allocate the chance_kind_table */
	C_MAKE(chance_kind_table, z_info->k_max, byte);

	/* Allow all legal objects */
	for (i = 0; i < z_info->k_max; i++) permit_kind_table[i] = TRUE;

	/*** Analyze monster allocation info ***/

	/* Clear the "aux" array */
	(void)C_WIPE(&aux, MAX_DEPTH, s16b);

	/* Clear the "num" array */
	(void)C_WIPE(&num, MAX_DEPTH, s16b);

	/* Size of "alloc_race_table" */
	alloc_race_size = 0;

	/* Scan the monsters (not the ghost) */
	for (i = 1; i < z_info->r_max - 1; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[i];

		/* Legal monsters */
		if (r_ptr->rarity)
		{
			/* Count the entries */
			alloc_race_size++;

			/* Group by level */
			num[r_ptr->level]++;
		}
	}

	/* Collect the level indexes */
	for (i = 1; i < MAX_DEPTH; i++)
	{
		/* Group by level */
		num[i] += num[i-1];
	}

	/* Paranoia */
	if (!num[0]) quit("No town monsters!");


	/*** Initialize monster allocation info ***/

	/* Allocate the alloc_race_table */
	C_MAKE(alloc_race_table, alloc_race_size, alloc_entry);

	/* Get the table entry */
	table = alloc_race_table;

	/* Scan the monsters (not the ghost) */
	for (i = 1; i < z_info->r_max - 1; i++)
	{
		/* Get the i'th race */
		r_ptr = &r_info[i];

		/* Count valid pairs */
		if (r_ptr->rarity)
		{
			int p, x, y, z;

			/* Extract the base level */
			x = r_ptr->level;

			/* Extract the base probability */
			p = (100 / r_ptr->rarity);

			/* Skip entries preceding our locale */
			y = (x > 0) ? num[x-1] : 0;

			/* Skip previous entries at this locale */
			z = y + aux[x];

			/* Load the entry */
			table[z].index = i;
			table[z].level = x;
			table[z].prob1 = p;
			table[z].prob2 = p;
			table[z].prob3 = p;

			/* Another entry complete for this locale */
			aux[x]++;
		}
	}

	/*** Analyze ego_item allocation info ***/

	/* Clear the "aux" array */
	(void)C_WIPE(&aux, MAX_DEPTH, s16b);

	/* Clear the "num" array */
	(void)C_WIPE(&num, MAX_DEPTH, s16b);

	/* Size of "alloc_ego_table" */
	alloc_ego_size = 0;

	/* Scan the ego items */
	for (i = 1; i < z_info->e_max; i++)
	{
		/* Get the i'th ego item */
		e_ptr = &e_info[i];

		/* Legal items */
		if (e_ptr->rarity)
		{
			/* Count the entries */
			alloc_ego_size++;

			/* Group by level */
			num[e_ptr->level]++;
		}
	}

	/* Collect the level indexes */
	for (i = 1; i < MAX_DEPTH; i++)
	{
		/* Group by level */
		num[i] += num[i-1];
	}

	/*** Initialize ego-item allocation info ***/

	/* Allocate the alloc_ego_table */
	C_MAKE(alloc_ego_table, alloc_ego_size, alloc_entry);

	/* Get the table entry */
	table = alloc_ego_table;

	/* Scan the ego-items */
	for (i = 1; i < z_info->e_max; i++)
	{
		/* Get the i'th ego item */
		e_ptr = &e_info[i];

		/* Count valid pairs */
		if (e_ptr->rarity)
		{
			int p, x, y, z;

			/* Extract the base level */
			x = e_ptr->level;

			/* Extract the base probability */
			p = (100 / e_ptr->rarity);

			/* Skip entries preceding our locale */
			y = (x > 0) ? num[x-1] : 0;

			/* Skip previous entries at this locale */
			z = y + aux[x];

			/* Load the entry */
			table[z].index = i;
			table[z].level = x;
			table[z].prob1 = p;
			table[z].prob2 = p;
			table[z].prob3 = p;

			/* Another entry complete for this locale */
			aux[x]++;
		}
	}

	/*** Initialize quest monsters ***/

	/* Scan the quests */
	for (i = 0; i < z_info->q_max; i++)
	{
		/* Get the i'th quest */
		q_ptr = &q_info[i];

		/* Skip non-quests */
		if (q_ptr->active_level)
		{
			/* Get the quest monster */
			r_ptr = &r_info[q_ptr->mon_idx];
		}
	}

	/* Success */
	return (0);
}


/*
 * Hack -- take notes on line 23
 */
static void note(cptr str)
{
	Term_erase(0, 23, 255);
	Term_putstr(20, 23, -1, TERM_WHITE, str);
	Term_fresh();
}



/*
 * Hack -- Explain a broken "lib" folder and quit (see below).
 *
 * XXX XXX XXX This function is "messy" because various things
 * may or may not be initialized, but the "plog()" and "quit()"
 * functions are "supposed" to work under any conditions.
 */
static void init_angband_aux(cptr why)
{
	/* Why */
	plog(why);

	/* Explain */
	plog("The 'lib' directory is probably missing or broken.");

	/* More details */
	plog("Perhaps the archive was not extracted correctly.");

	/* Explain */
	plog("See the 'README' file for more information.");

	/* Quit with error */
	quit("Fatal Error.");
}


/*
 * Hack -- main Angband initialization entry point
 *
 * Verify some files, display the "news.txt" file, create
 * the high score file, initialize all internal arrays, and
 * load the basic "user pref files".
 *
 * Be very careful to keep track of the order in which things
 * are initialized, in particular, the only thing *known* to
 * be available when this function is called is the "z-term.c"
 * package, and that may not be fully initialized until the
 * end of this function, when the default "user pref files"
 * are loaded and "Term_xtra(TERM_XTRA_REACT,0)" is called.
 *
 * Note that this function attempts to verify the "news" file,
 * and the game aborts (cleanly) on failure, since without the
 * "news" file, it is likely that the "lib" folder has not been
 * correctly located.  Otherwise, the news file is displayed for
 * the user.
 *
 * Note that this function attempts to verify (or create) the
 * "high score" file, and the game aborts (cleanly) on failure,
 * since one of the most common "extraction" failures involves
 * failing to extract all sub-directories (even empty ones), such
 * as by failing to use the "-d" option of "pkunzip", or failing
 * to use the "save empty directories" option with "Compact Pro".
 * This error will often be caught by the "high score" creation
 * code below, since the "lib/apex" directory, being empty in the
 * standard distributions, is most likely to be "lost", making it
 * impossible to create the high score file.
 *
 * Note that various things are initialized by this function,
 * including everything that was once done by "init_some_arrays".
 *
 * This initialization involves the parsing of special files
 * in the "lib/data" and sometimes the "lib/edit" directories.
 *
 * Note that the "template" files are initialized first, since they
 * often contain errors.  This means that macros and message recall
 * and things like that are not available until after they are done.
 *
 * We load the default "user pref files" here in case any "color"
 * changes are needed before character creation.
 *
 * Note that the "graf-xxx.prf" file must be loaded separately,
 * if needed, in the first (?) pass through "TERM_XTRA_REACT".
 */
void init_angband(void)
{
	int fd;

	int mode = 0644;

	FILE *fp;

	char buf[1024];


	/*** Verify the "news" file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "news.txt");

	/* Attempt to open the file */
	fd = fd_open(buf, O_RDONLY);

	/* Failure */
	if (fd < 0)
	{
		char why[1024];

		/* Message */
		sprintf(why, "Cannot access the '%s' file!", buf);

		/* Crash and burn */
		init_angband_aux(why);
	}

	/* Close it */
	fd_close(fd);


	/*** Display the "news" file ***/

	/* Clear screen */
	Term_clear();

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "news.txt");

	/* Open the News file */
	fp = my_fopen(buf, "r");

	/* Dump */
	if (fp)
	{
		int i = 0;

		/* Dump the file to the screen */
		while (0 == my_fgets(fp, buf, 1024))
		{
			/* Display and advance */
			Term_putstr(0, i++, -1, TERM_WHITE, buf);
		}

		/* Close */
		my_fclose(fp);
	}

	/* Flush it */
	Term_fresh();


	/*** Verify (or create) the "high score" file ***/

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_APEX, "scores.raw");

	/* Attempt to open the high score file */
	fd = fd_open(buf, O_RDONLY);

	/* Failure */
	if (fd < 0)
	{
		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Grab permissions */
		safe_setuid_grab();

		/* Create a new high score file */
		fd = fd_make(buf, mode);

		/* Drop permissions */
		safe_setuid_drop();

		/* Failure */
		if (fd < 0)
		{
			char why[1024];

			/* Message */
			sprintf(why, "Cannot create the '%s' file!", buf);

			/* Crash and burn */
			init_angband_aux(why);
		}
	}

	/* Close it */
	fd_close(fd);


	/*** Initialize some arrays ***/

	/* Initialize size info */
	note("[Initializing array sizes...]");
	if (init_z_info()) quit("Cannot initialize sizes");

	/* Initialize feature info */
	note("[Initializing arrays... (features)]");
	if (init_f_info()) quit("Cannot initialize features");

	/* Initialize object info */
	note("[Initializing arrays... (objects)]");
	if (init_k_info()) quit("Cannot initialize objects");

	/* Initialize artifact info */
	note("[Initializing arrays... (artifacts)]");
	if (init_a_info()) quit("Cannot initialize artifacts");

	/* Initialize ego-item info */
	note("[Initializing arrays... (ego-items)]");
	if (init_e_info()) quit("Cannot initialize ego-items");

	/* Initialize monster info */
	note("[Initializing arrays... (monsters)]");
	if (init_r_info()) quit("Cannot initialize monsters");

	/* Initialize feature info */
	note("[Initializing arrays... (vaults)]");
	if (init_v_info()) quit("Cannot initialize vaults");

	/* Initialize history info */
	note("[Initializing arrays... (histories)]");
	if (init_h_info()) quit("Cannot initialize histories");

	/* Initialize race info */
	note("[Initializing arrays... (races)]");
	if (init_p_info()) quit("Cannot initialize races");

	/* Initialize class info */
	note("[Initializing arrays... (classes)]");
	if (init_c_info()) quit("Cannot initialize classes");

	/* Initialize owner info */
	note("[Initializing arrays... (owners)]");
	if (init_b_info()) quit("Cannot initialize owners");

	/* Initialize price info */
	note("[Initializing arrays... (prices)]");
	if (init_g_info()) quit("Cannot initialize prices");

	/* Initialize quest info */
	note("[Initializing arrays... (quests)]");
	if (init_q_info()) quit("Cannot initialize quests");

	/* Initialize some other arrays */
	note("[Initializing arrays... (other)]");
	if (init_other()) quit("Cannot initialize other stuff");

	/* Initialize some other arrays */
	note("[Initializing arrays... (alloc)]");
	if (init_alloc()) quit("Cannot initialize alloc stuff");


	/*** Load default user pref files ***/

	/* Initialize feature info */
	note("[Loading basic user pref file...]");

	/* Process that file */
	(void)process_pref_file("pref.prf");

	/* Done */
	note("[Initialization complete]");
}


void cleanup_angband(void)
{
	int i, j;

	/* Free the macros */
	for (i = 0; i < macro__num; ++i)
	{
		string_free(macro__pat[i]);
		string_free(macro__act[i]);
	}

	C_FREE((void*)macro__pat, MACRO_MAX, cptr);
	C_FREE((void*)macro__act, MACRO_MAX, cptr);

	/* Free the keymaps */
	for (i = 0; i < KEYMAP_MODES; ++i)
	{
		for (j = 0; j < 256; ++j)
		{
			string_free(keymap_act[i][j]);
		}
	}

	/* Free the allocation tables */
	C_FREE(alloc_ego_table, alloc_ego_size, alloc_entry);
	C_FREE(alloc_race_table, alloc_race_size, alloc_entry);
	FREE(permit_kind_table, alloc_entry);
	FREE(chance_kind_table, alloc_entry);
	
	if (store)
	{
		/* Free the store inventories */
		for (i = 0; i < MAX_STORES; i++)
		{
			/* Get the store */
			store_type *st_ptr = &store[i];

			/* Free the store inventory */
			C_FREE(st_ptr->stock, st_ptr->stock_size, object_type);

			/* No "legal items" for the black market or home */
			if ((i == STORE_B_MARKET) || (i == STORE_HOME)) continue;

			/* Free the legal item choices */
			C_FREE(st_ptr->table, STORE_CHOICES, s16b);
		}
	}

	/* Free the stores */
	C_FREE(store, MAX_STORES, store_type);

	/* Free the player inventory */
	C_FREE(inventory, INVEN_TOTAL, object_type);

	/* Free the lore, monster, and object lists */
	C_FREE(l_list, z_info->r_max, monster_lore);
	C_FREE(m_list, z_info->m_max, monster_type);
	C_FREE(o_list, z_info->o_max, object_type);

#ifdef MONSTER_FLOW

	/* Flow arrays */
	C_FREE(cave_when, DUNGEON_HGT, byte_wid);
	C_FREE(cave_cost, DUNGEON_HGT, byte_wid);

#endif /* MONSTER_FLOW */

	/* Free the cave */
	C_FREE(cave_o_idx, DUNGEON_HGT, s16b_wid);
	C_FREE(cave_m_idx, DUNGEON_HGT, s16b_wid);
	C_FREE(cave_feat, DUNGEON_HGT, byte_wid);
	C_FREE(cave_info, DUNGEON_HGT, byte_256);

	/* Free the "update_view()" array */
	C_FREE(view_g, VIEW_MAX, u16b);

	/* Free the temp array */
	C_FREE(temp_g, TEMP_MAX, u16b);

	/* Free the messages */
	messages_free();

	/* Free the "quarks" */
	quarks_free();

	/* Free the info, name, and text arrays */
	free_info(&g_head);
	free_info(&b_head);
	free_info(&p_head);
	free_info(&h_head);
	free_info(&v_head);
	free_info(&r_head);
	free_info(&e_head);
	free_info(&a_head);
	free_info(&k_head);
	free_info(&f_head);
	free_info(&z_head);

	/* Free the format() buffer */
	vformat_kill();

	/* Free the directories */
	string_free(ANGBAND_DIR);
	string_free(ANGBAND_DIR_APEX);
	string_free(ANGBAND_DIR_BONE);
	string_free(ANGBAND_DIR_DATA);
	string_free(ANGBAND_DIR_EDIT);
	string_free(ANGBAND_DIR_FILE);
	string_free(ANGBAND_DIR_HELP);
	string_free(ANGBAND_DIR_INFO);
	string_free(ANGBAND_DIR_SAVE);
	string_free(ANGBAND_DIR_PREF);
	string_free(ANGBAND_DIR_USER);
	string_free(ANGBAND_DIR_XTRA);
}

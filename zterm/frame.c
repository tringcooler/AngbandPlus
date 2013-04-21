#include "langband.h"
#include "lbwindows.h"

int num_predefinedFrames = -1;
int max_predefinedFrames = -1;
LangbandFrame **predefinedFrames = NULL;
int num_activeFrames = -1;
int max_activeFrames = -1;
LangbandFrame **activeFrames = NULL;

int
init_frame_system(int active_size, int predefined_size) {
    int i;

    //DBGPUT("Sizes %d and %d\n", active_size, predefined_size);
    
    if (active_size > 0) {
	max_activeFrames = active_size;
	num_activeFrames = 0;
	activeFrames = malloc(active_size * sizeof(LangbandFrame*));
	for (i=0; i < active_size; i++) {
	    activeFrames[i] = NULL;
	}
    }
    else {
	ERRORMSG("Illegal size %d for active frame/subwindow-array.\n", active_size);
	return -1;
    }

    if (predefined_size > 0) {
	max_predefinedFrames = predefined_size;
	num_predefinedFrames = predefined_size; 
	predefinedFrames = malloc(predefined_size * sizeof(LangbandFrame*));
	for (i=0; i < predefined_size; i++) {
	    predefinedFrames[i] = NULL;
	}
    }
    else {
	ERRORMSG("Illegal size %d for predefined frame/subwindow-array.\n", active_size);
	return -2;
    }

    return 0;
}

int
cleanup_frame_system() {
    int i;

    //DBGPUT("Cleaning frame-system\n");
    
    if (max_predefinedFrames > 0) {
        for (i=0; i < max_predefinedFrames; i++) {
	    if (predefinedFrames[i]) {
		free(predefinedFrames[i]);
		predefinedFrames[i] = NULL;
	    }
	}
	free(predefinedFrames);
	predefinedFrames = NULL;
	
	max_predefinedFrames = num_predefinedFrames = -1;
    }

    if (max_activeFrames > 0) {
        for (i=0; i < max_activeFrames; i++) {
	    if (activeFrames[i]) {
		// just pointers to already cleaned predefined frames
		//free(activeFrames[i]);
		activeFrames[i] = NULL;
	    }
	}
	free(activeFrames);
	activeFrames = NULL;
	
	max_activeFrames = num_activeFrames = -1;
    }
    
    return 0;
}

int
legal_frame_key_p(int key, FrameType ft) {
    int cur_size = -1;
    
    if (ft == ACTIVE) {
	cur_size = max_activeFrames;
    }
    else if (ft == PREDEFINED) {
	cur_size = max_predefinedFrames;
    }
    else {
	ERRORMSG("Illegal frametype %d\n", ft);
    }
    
    if (key >= 0 && key < cur_size) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

LangbandFrame *
make_frame(int key, const char *name) {
    if (!name) {
	ERRORMSG("Empty name passed to make_frame(), please supply a real name.\n");
	return NULL;
    }
    
    if (legal_frame_key_p(key, PREDEFINED)) {
	LangbandFrame *lf = malloc(sizeof(LangbandFrame));
	memset(lf, 0, sizeof(LangbandFrame));
	lf->key = key;
	lf->name = malloc(strlen(name)+1);
	strcpy(lf->name, name);
	//DEBUGPUT("Made frame '%s' at %d\n", name, key);
	return lf;
    }
    else {
	ERRORMSG("Illegal key %d supplied to make_frame() %s.\n", key, name);
	return NULL;
    }
}

int
add_frame(int key, const char *name) {
    
    LangbandFrame *lf = make_frame(key, name);
    
    if (!lf) {
	ERRORMSG("Unable to produce a LangbandFrame [%d,%s], returning.\n",
		 key, name);
	return -1;
    }

    //DBGPUT("Added frame %s at %d.\n", name, key);
    predefinedFrames[key] = lf;
    
    return 0;
}


int
add_frame_coords(int key, int x, int y, int w, int h) {

    if (!legal_frame_key_p(key,PREDEFINED)) {
	ERRORMSG("Illegal key %d for frame.\n", key);
	return -1;
    }
    else {
    	LangbandFrame *lf = predefinedFrames[key];

	if (!lf) {
	    ERRORMSG("No frame registered for key key %d.\n", key);
	    return -2;   
	}
	
	lf->xoffset = x;
	lf->yoffset = y;
	lf->allowed_width = w;
	lf->allowed_height = h;
	
	return 0;
    }
}

int
add_frame_tileinfo(int key, int tw, int th, const char *font) {

    if (legal_frame_key_p(key, PREDEFINED)) {
	LangbandFrame *lf = predefinedFrames[key];

	lf->tile_width = tw;
	lf->tile_height = th;
	if (font && strlen(font)>1) {
	    lf->fontname = malloc(strlen(font)+1);
	    strcpy(lf->fontname, font);
	}
//	if (bg && strlen(bg) > 1) {
//	    lf->backgroundfile = malloc(strlen(bg)+1);
//	    strcpy(lf->backgroundfile, bg);
//	}
		
	return 0;
    }
    else {
	ERRORMSG("Illegal key %d for subwindow.\n", key);
	return -1;
    }
}

int
add_frame_gfxinfo(int key, int use_tiles) {

    if (legal_frame_key_p(key, PREDEFINED)) {
	LangbandFrame *lf = predefinedFrames[key];

	lf->use_gfx_tiles = use_tiles;
		
	return 0;
    }
    else {
	ERRORMSG("Illegal key %d for subwindow.\n", key);
	return -1;
    }
}

int
add_frame_bg(int key, int img_idx) {
    if (legal_frame_key_p(key, PREDEFINED)) {
	LangbandFrame *lf = predefinedFrames[key];

	if (!lf) {
	    return -2;
	}
	else {
	    lf->background = img_idx;
	    return 0;
	}
    }
    else {
	ERRORMSG("Illegal key %d for subwindow.\n", key);
	return -1;
    }

}


LangbandFrame *
get_frame(int key, FrameType ft) {
    //DBGPUT("Getting frame %d %d which is %p %p.\n", key, ft, activeFrames[key], predefinedFrames[key]);
    if (legal_frame_key_p(key, ft)) {
	if (ft == ACTIVE) {
	    return activeFrames[key];
	}
	else if (ft == PREDEFINED) {
	    return predefinedFrames[key];
	}
	else {
	    ERRORMSG("Illegal frametype %d for frame %d.\n", ft, key);
	    return NULL;
	}
    }

    ERRORMSG("Illegal key %d for subwindow.\n", key);
    return NULL;
}

int
has_frame(int key, FrameType ft) {
    LangbandFrame *lf = get_frame(key, ft);
    if (lf)
	return TRUE;
    else
	return FALSE;
}

int
get_frame_columns(int key, FrameType ft) {
    LangbandFrame *lf = get_frame(key, ft);
    if (lf)
	return lf->columns;
    else
	return -50;
}

int
get_frame_rows(int key, FrameType ft) {
    LangbandFrame *lf = get_frame(key, ft);
    if (lf)
	return lf->rows;
    else
	return -50;
}

int
get_frame_tile_width(int key, FrameType ft) {
    LangbandFrame *lf = get_frame(key, ft);
    if (lf)
	return lf->tile_width;
    else
	return -50;
}

int
get_frame_tile_height(int key, FrameType ft) {
    LangbandFrame *lf = get_frame(key, ft);
    if (lf)
	return lf->tile_height;
    else
	return -50;
}

int
get_frame_gfx_tiles(int key, FrameType ft) {
    LangbandFrame *lf = get_frame(key, ft);
    if (lf)
	return lf->use_gfx_tiles;
    else
	return -50;
}

int
clean_frame(int key) {
    LangbandFrame *lf = get_frame(key, ACTIVE);
    if (lf && lf->visible) {
	DEBUGPUT("CLEANING FRAME.. BAD!");
#if 0
	angband_zterm *old = Term;
	Term_activate(lf->azt);
	Term_redraw();
	Term_fresh();
	Term_activate(old);
#endif
	return key;
    }
    else {
	return -1;
    }
}

int
wipe_frame(int key) {
    LangbandFrame *lf = get_frame(key, ACTIVE);

    if (lf && lf->visible) {
	DBGPUT("Wiping FRAME %d.\n", key);
#if 0
	angband_zterm *old = Term;
	int which_ui = current_ui();
	

	
	Term_activate(lf->azt);
	
	Term_clear();
	Term_fresh();
	
	if (FALSE) { return -1; }
#ifdef USE_SDL
	else if (which_ui == UITYPE_SDL) {
	    Term_xtra_sdl(TERM_XTRA_CLEAR,5);
	}
#endif
#ifdef USE_GCU
	else if (which_ui == UITYPE_GCU) {
	    Term_xtra_gcu(TERM_XTRA_CLEAR,5);
	}
#endif

	// redraw here?
	Term_redraw();
	Term_fresh();
	Term_activate(old);
	//DBGPUT("Wiped %d.\n", key);
#endif
	return key;
    }
    else {
	return -1;
    }
}

int
activate_frame(int key) {
    LangbandFrame *lf = NULL;

    if (!legal_frame_key_p(key, PREDEFINED)) {
        ERRORMSG("Illegal key %d given to activate_frame().\n", key);
        return -1;
    }

    //DBGPUT("Activate frame %d.\n", key);
    
    lf = predefinedFrames[key];

    if (lf) {
        lf->visible = TRUE;
        activeFrames[key] = lf;
        num_activeFrames++;
        return key;
    }
    else {
        ERRORMSG("Illegal key %d given to activate_frame() (doesn't exist).\n", key);
        return -2;
    }
}

int
deactivate_frame(int key) {
    LangbandFrame *lf = NULL;
    
    if (!legal_frame_key_p(key, PREDEFINED)) {
        ERRORMSG("Illegal key %d given to activate_frame().\n", key);
        return -1;
    }
    
    //DBGPUT("Deactivate frame %d.\n", key);
    
    lf = predefinedFrames[key];

    if (lf) {
        lf->visible = FALSE;
        activeFrames[key] = NULL;
        num_activeFrames--;
        return key;
    }
    else {
        // it's ok to deactivate a frame that's not there
        return -2;
    }
}

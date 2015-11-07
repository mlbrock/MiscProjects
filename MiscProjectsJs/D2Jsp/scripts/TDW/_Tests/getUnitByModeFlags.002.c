// You probably have these already...
#define D2JSP_CORE_PLAYER	0
#define D2JSP_CORE_NPC		1
#define D2JSP_CORE_OBJECT	2
#define D2JSP_CORE_ITEM		4

#define D2JSP_MAX_ANY_MODE	104			// Item mode 'in stash'.
#define D2JSP_MODE_COUNT	105

// Player modes...
static const unsigned int D2JSP_CORE_ModeFlagToValue_Player[32][2] = {
	{	0x0001,   0	},				// Death
	{	0x0002,   1	},				// Standing outside of town
	{	0x0004,   2	},				// Walking
		// Et cetera...
};
	// NPC modes...
static const unsigned int D2JSP_CORE_ModeFlagToValue_NPC[32][2]    = {
};
	// Object modes...
static const unsigned int D2JSP_CORE_ModeFlagToValue_Object[32][2] = {
};

	// Item modes...
static const unsigned int D2JSP_CORE_ModeFlagToValue_Item[32][2]   = {
};

void D2JSP_CORE_BuildModeFlagMatchSpec(int unit_type, unsigned int mode_flags,
	unsigned int *mode_map_list)
{
	unsigned int        count_1;
	const unsigned int *mode_list;

	// Too bad these aren't numbered sequentially...
	if (unit_type == D2JSP_CORE_PLAYER)
		mode_list = D2JSP_CORE_ModeFlagToValue_Player;
	else (unit_type == D2JSP_CORE_NPC)
		mode_list = D2JSP_CORE_ModeFlagToValue_NPC;
	else (unit_type == D2JSP_CORE_OBJECT)
		mode_list = D2JSP_CORE_ModeFlagToValue_Object;
	else (unit_type == D2JSP_CORE_ITEM)
		mode_list = D2JSP_CORE_ModeFlagToValue_Item;
	else
		return;

	for (count_1 = 0; mode flags && (count_1 < (sizeof(unsigned int) * CHAR_BIT));
		count_1++) {
		if (mode_list[count_1][0] & mode_flags) {
			mode_map_list[mode_map[count_1][1]]  = true;
			mode_flags                          -= mode_list[count_1][0];
		}
	}
}

// Supports JS getUnitByModeFlags()
const ptUnit *D2JSP_CORE_getUnitByModeFlags(unsigned int unit_type,
	const char *unit_id, unsigned mode_flags)
{
	const ptUnit *ptunit_ptr = getFirstPTUnitPtr();
	unsigned int  mode_map_list[D2JSP_MODE_COUNT];

	D2JSP_CORE_BuildModeFlagMatchSpec(unit_type, mode_flags, mode_map_list);

	while (/* Whatever */) {
		// Check unit for match...
		if (ptunit_ptr->type == unit_type) {
			if (mode_map_list[ptunit_ptr->mode])
				return(ptunit_ptr);
		}
		// Otherwise, grab the next unit...
		ptunit_ptr = ptunit_ptr->next_ptr;	// Assumes linked list.
	}

	return(NULL);
}

// Supports JS getNext()
const ptUnit *D2JSP_CORE_getNextByModeFlags(const ptUnit *ptunit_ptr,
	const char *unit_id, unsigned mode_flags)
{
	unsigned int  mode_map_list[D2JSP_MODE_COUNT];

	D2JSP_CORE_BuildModeFlagMatchSpec(unit_type, mode_flags, mode_map_list);

	while (/* Whatever */) {
		// Check unit for match...
		if (ptunit_ptr->type == unit_type) {
			if (mode_map_list[ptunit_ptr->mode])
				return(ptunit_ptr);
		}
		// Otherwise, grab the next unit...
		ptunit_ptr = ptunit_ptr->next_ptr;	// Assumes linked list.
	}

	return(NULL);
}


// NOTE: buffer names are aliases to generated names
// this is a bit of a hack

// TODO: adhesion can be based on membrane field strength

#define Display data_0

#define Centers data_1

#define CytoplasmField data_2
#define CytoplasmId data_3

#define Membrane data_4
#define MembraneField data_5

#define BufA data_6
#define BufB data_7

// NOTE: those values are random and bad
#define MEMB_SEUIL 10
#define MOVE_SEUIL 0x100

struct CellInfo
{
    int cytoplasm_field_strength;
    int membrane_threshold;
    int default_membrane; // loose membranes
    int cytoplasm_color;
};

struct MembInfo
{
    int field_strength;
    int membrane_color;
};

#define CELL_TYPE_COUNT 2

const MembInfo memb_info[] = MembInfo[](
    MembInfo(0x003FFFF, 0xFFBFBF00), // base membrane (used for defaults)
    MembInfo(0x000FFFF, 0xFF00BFBF), // sticky membrane (low strength)
    MembInfo(0x00FFFFF, 0xFFBF00BF)  // pushy membrane (high strength)
);

const CellInfo cell_info[CELL_TYPE_COUNT] = CellInfo[](
    CellInfo(0x0FFFFFFF, 10, 0 /* base */, 0xFF7F007F),
    CellInfo(0x0FFFFFFF, 10, 0 /* base */, 0xFF7F7F00)
);

// memb_table[self_type_id][neighbor_type_id]
const int memb_table[CELL_TYPE_COUNT][CELL_TYPE_COUNT] = int[CELL_TYPE_COUNT][](
    int[](1, 2),
    int[](2, 1)
);

int get_cell_type(int id)
{
    return id & 1;
}

int check_membrane(int id, int threshold, int p)
{
    return (CytoplasmId[p] != id || CytoplasmField[p] < threshold) ? 1 : 0;
}

int get_membrane(int self_id, int threshold, int p)
{
    int id = CytoplasmId[p];

    return CytoplasmField[p] < threshold
        ? self_id
        : (id != self_id
            ? id : 0);
}

ivec2 unpack_ivec2(int val)
{
    ivec2 res = ivec2((val & 0xFFFF), (val & 0xFFFF0000) >> 16);

    if ((res.x & 0x8000) != 0) { res.x |= 0xFFFF0000; }
    if ((res.y & 0x8000) != 0) { res.y |= 0xFFFF0000; }

    return res;
}

int pack_ivec2(ivec2 val)
{
    return ((val.x) & 0xFFFF) | ((val.y << 16) & 0xFFFF0000);
}

void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);

    // generate initial config
    if (current_step == 0)
    {
        Centers[p] = 0;
        Display[p] = 0;
    }
    else
    {
        int xl = int(uint(x - 1 + WSX) % WSX);
        int xr = int(uint(x + 1) % WSX);
        int yu = int(uint(y - 1 + WSY) % WSY);
        int yd = int(uint(y + 1) % WSY);

        int pmu = x + yu * int(WSX);
        int pmd = x + yd * int(WSX);
        int plm = xl + y * int(WSX);
        int prm = xr + y * int(WSX);

        if (current_pass == PASS_INIT)
        {
            if (x == click_x && y == click_y)
            {
                Centers[p] = current_step;
            }

            CytoplasmId[p] = Centers[p];

            if (Centers[p] != 0)
            {
                int cell_type_here = get_cell_type(Centers[p]);
                CytoplasmField[p] = cell_info[cell_type_here].cytoplasm_field_strength;
            }
            else
            {
                CytoplasmField[p] = 0;
            }

            Membrane[p] = 0;
        }

        if (current_pass >= PASS_CYTO_INIT && current_pass <= PASS_CYTO_FINI)
        {
            // HACK, this could be done more elegantly by swapping the input buffers every other pass
            if (((current_pass - PASS_CYTO_INIT) & 1) == 1)
            {
                CytoplasmField[p] = BufA[p];
            }
            else
            {
                int id = CytoplasmId[p];
                int mx = CytoplasmField[p];

                if (CytoplasmField[pmu] > mx) { id = CytoplasmId[pmu]; mx = CytoplasmField[pmu]; }
                if (CytoplasmField[pmd] > mx) { id = CytoplasmId[pmd]; mx = CytoplasmField[pmd]; }
                if (CytoplasmField[plm] > mx) { id = CytoplasmId[plm]; mx = CytoplasmField[plm]; }
                if (CytoplasmField[prm] > mx) { id = CytoplasmId[prm]; mx = CytoplasmField[prm]; }

                BufA[p] = CytoplasmField[p] / 10 * 6
                    + CytoplasmField[pmu] / 10
                    + CytoplasmField[pmd] / 10
                    + CytoplasmField[plm] / 10
                    + CytoplasmField[prm] / 10;

                CytoplasmId[p] = id;
            }
        }

        if (current_pass == PASS_MEMB)
        {
            Membrane[p] = 0;
            MembraneField[p] = 0;

            int id_here = CytoplasmId[p];
            int type_here = get_cell_type(CytoplasmId[p]);
            int threshold = cell_info[type_here].membrane_threshold;

            if (CytoplasmField[p] >= threshold)
            {
                ivec4 neighbors = ivec4(
                    get_membrane(id_here, threshold, pmu),
                    get_membrane(id_here, threshold, plm),
                    get_membrane(id_here, threshold, prm),
                    get_membrane(id_here, threshold, pmd));

                ivec2 neighbors2 = max(neighbors.xy, neighbors.zw);
                int neighbor_id = max(neighbors2.x, neighbors2.y);

                /*
                int m = 0;

                m += check_membrane(id_here, threshold, pmu);
                m += check_membrane(id_here, threshold, plm);
                m += check_membrane(id_here, threshold, prm);
                m += check_membrane(id_here, threshold, pmd);
                */

                if (neighbor_id != 0)
                {
                    int self_type = get_cell_type(id_here);
                    int memb_type = 0;

                    if (neighbor_id == id_here)
                    {
                        memb_type = cell_info[self_type].default_membrane;
                    }
                    else
                    {
                        int neighbor_type = get_cell_type(neighbor_id);
                        memb_type = memb_table[self_type][neighbor_type];
                    }

                    Membrane[p] = memb_type + 1; // + 1 to distinguish from none if type == 0
                    MembraneField[p] = memb_info[memb_type].field_strength;
                }
            }
        }

        if (current_pass >= PASS_MEMB_INIT && current_pass <= PASS_MEMB_FINI)
        {
            if (((current_pass - PASS_MEMB_INIT) & 1) == 1)
            {
                MembraneField[p] = BufA[p] / 10 * 6
                    + BufA[pmu] / 10
                    + BufA[pmd] / 10
                    + BufA[plm] / 10
                    + BufA[prm] / 10;
            }
            else
            {
                BufA[p] = MembraneField[p] / 10 * 6
                    + MembraneField[pmu] / 10
                    + MembraneField[pmd] / 10
                    + MembraneField[plm] / 10
                    + MembraneField[prm] / 10;
            }
        }

        if (current_pass == PASS_DIRECTION)
        {
            int dir = 0;
            int mx = 0;

            if (MembraneField[pmu] > MembraneField[pmd] + MOVE_SEUIL)
            {
                if (mx < MembraneField[pmu]) { mx = MembraneField[pmu]; dir = 1; }
            }

            if (MembraneField[pmd] > MembraneField[pmu] + MOVE_SEUIL)
            {
                if (mx < MembraneField[pmd]) { mx = MembraneField[pmd]; dir = 2; }
            }

            if (MembraneField[plm] > MembraneField[prm] + MOVE_SEUIL)
            {
                if (mx < MembraneField[plm]) { mx = MembraneField[plm]; dir = 3; }
            }

            if (MembraneField[prm] > MembraneField[plm] + MOVE_SEUIL)
            {
                if (mx < MembraneField[prm]) { mx = MembraneField[prm]; dir = 4; }
            }

            BufA[p] = dir;
        }

        if (current_pass == PASS_MOVE)
        {
            if (Centers[p] != 0)
            {
                if (BufA[p] != 0)
                    BufB[p] = 0;
                else
                    BufB[p] = Centers[p];
            }
            else
            {
                     if (BufA[pmu] == 1 && Centers[pmu] != 0) { BufB[p] = Centers[pmu]; }
                else if (BufA[pmd] == 2 && Centers[pmd] != 0) { BufB[p] = Centers[pmd]; }
                else if (BufA[plm] == 3 && Centers[plm] != 0) { BufB[p] = Centers[plm]; }
                else if (BufA[prm] == 4 && Centers[prm] != 0) { BufB[p] = Centers[prm]; }
                else { BufB[p] = 0; }
            }
        }

        if (current_pass == PASS_MOVE_2)
        {
            Centers[p] = BufB[p];
        }

        if (current_pass == PASS_LAST)
        {
            // all of this is not very efficient

            Display[p] = 0xFF000000;

            if (display_mode == 0)
            {
                if (CytoplasmId[p] != 0)
                {
                    int type_here = get_cell_type(CytoplasmId[p]);

                    if (CytoplasmField[p] > cell_info[type_here].membrane_threshold)
                    {
                        Display[p] = cell_info[type_here].cytoplasm_color;
                    }
                }
            }

            if (display_mode == 0 || display_mode == 3)
            {
                if (Membrane[p] != 0)
                {
                    Display[p] = memb_info[Membrane[p] - 1].membrane_color; // 0xFF00BFBF;
                }
            }

            if (display_mode == 1)
            {
                if (CytoplasmField[p] != 0)
                {
                    int type_here = get_cell_type(CytoplasmId[p]);
                    int color_base = cell_info[type_here].cytoplasm_color & 0x010101;

                    // values that look good found empirically
                    float val = log2(float(CytoplasmField[p])) / log2(1.0675);
                    Display[p] += color_base * (int(val));
                }
            }

            if (display_mode == 0 || display_mode == 2)
            {
                if (Membrane[p] == 0 && MembraneField[p] > 0)
                {
                    Display[p] += 0x007F0000;
                }
            }

            if (display_mode == 4)
            {
                if (MembraneField[p] != 0)
                {
                    int color_base = 1; // red

                    // values that look good found empirically
                    float val = log2(float(MembraneField[p]) * 0x10) / log2(1.0675);
                    Display[p] += color_base * (int(val));
                }
            }

            if (Centers[p] != 0)
            {
                Display[p] = 0xFFFFFFFF;
            }
        }

        // MEMBRANE TEST VISU

        /*

        int type_here = get_cell_type(CytoplasmId[p]);

        if (CytoplasmField[p] > cell_info[type_here].membrane_threshold && MembraneField[p] > 0)
        {
            Display[p] = 0xFF000000 + MembraneField[p];
        }
        else
        {
            Display[p] = 0xFF000000;
        }

        */
    }
}

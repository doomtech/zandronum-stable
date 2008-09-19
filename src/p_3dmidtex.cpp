/*
** p_3dmidtex.cpp
**
** Eternity-style 3D-midtex handling
** (No original Eternity code here!)
**
**---------------------------------------------------------------------------
** Copyright 2008 Christoph Oelckers
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/


#include "templates.h"
#include "p_local.h"
#include "p_lnspec.h"


//============================================================================
//
// P_Scroll3dMidtex
//
// Scrolls all sidedefs belonging to 3dMidtex lines attached to this sector
//
//============================================================================

bool P_Scroll3dMidtex(sector_t *sector, int crush, fixed_t move, bool ceiling)
{
	extsector_t::midtex::plane &scrollplane = ceiling? sector->e->Midtex.Ceiling : sector->e->Midtex.Floor;

	// First step: Change all lines' texture offsets
	for(unsigned i = 0; i < scrollplane.AttachedLines.Size(); i++)
	{
		line_t *l = scrollplane.AttachedLines[i];

		sides[l->sidenum[0]].AddTextureYOffset(side_t::mid, move);
		sides[l->sidenum[1]].AddTextureYOffset(side_t::mid, move);
	}

	// Second step: Check all sectors whether the move is ok.
	bool res = false;

	for(unsigned i = 0; i < scrollplane.AttachedSectors.Size(); i++)
	{
		res |= P_ChangeSector(scrollplane.AttachedSectors[i], crush, move, 2, true);
	}
	return !res;
}

//============================================================================
//
// P_Start3DMidtexInterpolations
//
// Starts interpolators for every sidedef that is being changed by moving
// this sector
//
//============================================================================

void P_Start3dMidtexInterpolations(TArray<DInterpolation *> &list, sector_t *sector, bool ceiling)
{
	extsector_t::midtex::plane &scrollplane = ceiling? sector->e->Midtex.Ceiling : sector->e->Midtex.Floor;

	for(unsigned i = 0; i < scrollplane.AttachedLines.Size(); i++)
	{
		line_t *l = scrollplane.AttachedLines[i];

		list.Push(sides[l->sidenum[0]].SetInterpolation(side_t::mid));
		list.Push(sides[l->sidenum[1]].SetInterpolation(side_t::mid));
	}
}

//============================================================================
//
// P_Attach3dMidtexLinesToSector
//
// Attaches 3dMidtex lines to a sector.
// If lineid is != 0, all lines with the matching line id will be added
// If tag is != 0, all lines touching a sector with the matching tag will be added
// If both are != 0, all lines with the matching line id that touch a sector with
// the matching tag will be added.
//
//============================================================================

void P_Attach3dMidtexLinesToSector(sector_t *sector, int lineid, int tag, bool ceiling)
{
	int v;

	if (lineid == 0 && tag == 0)
	{
		// invalid set of parameters
		return;
	}

	extsector_t::midtex::plane &scrollplane = ceiling? sector->e->Midtex.Ceiling : sector->e->Midtex.Floor;

	// Bit arrays that mark whether a line or sector is to be attached.
	BYTE *found_lines = new BYTE[(numlines+7)/8];
	BYTE *found_sectors = new BYTE[(numsectors+7)/8];

	memset(found_lines, 0, sizeof (BYTE) * ((numlines+7)/8));
	memset(found_sectors, 0, sizeof (BYTE) * ((numsectors+7)/8));

	// mark all lines and sectors that are already attached to this one
	// and clear the arrays. The old data will be re-added automatically
	// from the marker arrays.
	for (unsigned i=0; i < scrollplane.AttachedLines.Size(); i++)
	{
		int line = int(scrollplane.AttachedLines[i] - lines);
		found_lines[line>>3] |= 1 << (line&7);
	}

	for (unsigned i=0; i < scrollplane.AttachedSectors.Size(); i++)
	{
		int sec = int(scrollplane.AttachedSectors[i] - sectors);
		found_sectors[sec>>3] |= 1 << (sec&7);
	}

	scrollplane.AttachedLines.Clear();
	scrollplane.AttachedSectors.Clear();

	if (tag == 0)
	{
		for(int line = -1; (line = P_FindLineFromID(lineid,line)) >= 0; )
		{
			line_t *ln = &lines[line];

			if (ln->frontsector == NULL || ln->backsector == NULL || !(ln->flags & ML_3DMIDTEX))
			{
				// Only consider two-sided lines with the 3DMIDTEX flag
				continue;
			}
			found_lines[line>>3] |= 1 << (line&7);
		}
	}
	else
	{
		for(int sec = -1; (sec = P_FindSectorFromTag(tag, sec)) >= 0; )
		{
			for (int line = 0; line < sectors[sec].linecount; line ++)
			{
				line_t *ln = sectors[sec].lines[line];

				if (lineid != 0 && ln->id != lineid) continue;

				if (ln->frontsector == NULL || ln->backsector == NULL || !(ln->flags & ML_3DMIDTEX))
				{
					// Only consider two-sided lines with the 3DMIDTEX flag
					continue;
				}
				found_lines[line>>3] |= 1 << (line&7);
			}
		}
	}


	for(int i=0; i < numlines; i++)
	{
		if (found_lines[i>>3] & (1 << (i&7)))
		{
			scrollplane.AttachedLines.Push(&lines[i]);

			v = int(lines[i].frontsector - sectors);
			found_sectors[v>>3] |= 1 << (v&7);

			v = int(lines[i].backsector - sectors);
			found_sectors[v>>3] |= 1 << (v&7);
		}
	}

	for (int i=0; i < numsectors; i++)
	{
		if (found_sectors[i>>3] & (1 << (i&7)))
		{
			scrollplane.AttachedSectors.Push(&sectors[i]);
		}
	}

	delete[] found_lines;
	delete[] found_sectors;
}


//============================================================================
//
// P_GetMidTexturePosition
//
// Retrieves top and bottom of the current line's mid texture.
//
//============================================================================
bool P_GetMidTexturePosition(const line_t *line, int sideno, fixed_t *ptextop, fixed_t *ptexbot)
{
	side_t *side = &sides[line->sidenum[sideno]];
	FTextureID texnum = side->GetTexture(side_t::mid);

	if (line->sidenum[0]==NO_SIDE || line->sidenum[1]==NO_SIDE || !texnum.isValid()) return false;
	
	FTexture * tex= TexMan(texnum);
	if (!tex) return false;

	fixed_t y_offset = side->GetTextureYOffset(side_t::mid);
	fixed_t textureheight = tex->GetScaledHeight() << FRACBITS;
	if (tex->yScale != FRACUNIT && !tex->bWorldPanning)
	{ 
		y_offset = FixedDiv(y_offset, tex->yScale);
	}

	if(line->flags & ML_DONTPEGBOTTOM)
	{
		*ptexbot = y_offset +
				   MAX<fixed_t>(line->frontsector->floortexz, line->backsector->floortexz);

		*ptextop = *ptexbot + textureheight;
	}
	else
	{
		*ptextop = y_offset +
				   MIN<fixed_t>(line->frontsector->ceilingtexz, line->backsector->ceilingtexz);
		
		*ptexbot = *ptextop - textureheight;
	}
	return true;
}


//============================================================================
//
// P_LineOpening_3dMidtex
//
// 3dMidtex part of P_LineOpening
//
//============================================================================

bool P_LineOpening_3dMidtex(AActor *thing, const line_t *linedef, fixed_t &opentop, fixed_t &openbottom)
{
	fixed_t tt, tb;

	if (P_GetMidTexturePosition(linedef, 0, &tt, &tb))
	{
		if (thing->z + (thing->height/2) < (tt + tb)/2)
		{
			if(tb < opentop) opentop = tb;
		}
		else
		{
			if(tt > openbottom) openbottom = tt;

			// returns true if it touches the midtexture
			return (abs(thing->z - tt) <= thing->MaxStepHeight);
		}
	}
	return false;

	/* still have to figure out what this code from Eternity means...
	if((linedef->flags & ML_BLOCKMONSTERS) && 
		!(mo->flags & (MF_FLOAT | MF_DROPOFF)) &&
		D_abs(mo->z - textop) <= 24*FRACUNIT)
	{
		opentop = openbottom;
		openrange = 0;
		return;
	}
	*/
}
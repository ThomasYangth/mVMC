/*
HPhi  -  Quantum Lattice Model Simulator
Copyright (C) 2015 The University of Tokyo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "StdFace_vals.h"
#include "StdFace_ModelUtil.h"
#include <complex.h>
#include "../include/wrapperMPI.h"

/**
*
* Clear grobal variables in the standard mode
*
* @author Mitsuaki Kawamura (The University of Tokyo)
*/
static void StdFace_ResetVals(struct StdIntList *StdI) {
  int i, j;
  /**/
  StdI->a = 9999.9;
  StdI->a0 = 9999.9;
  StdI->a0L = 9999;
  StdI->a0W = 9999;
  StdI->a1 = 9999.9;
  StdI->a1L = 9999;
  StdI->a1W = 9999;
  StdI->Gamma = 9999.9;
  StdI->h = 9999.9;
  StdI->JAll = 9999.9;
  StdI->JpAll = 9999.9;
  StdI->J0All = 9999.9;
  StdI->J1All = 9999.9;
  StdI->J1pAll = 9999.9;
  StdI->J2All = 9999.9;
  StdI->J2pAll = 9999.9;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      StdI->J[i][j] = 9999.9;
      StdI->Jp[i][j] = 9999.9;
      StdI->J0[i][j] = 9999.9;
      StdI->J1[i][j] = 9999.9;
      StdI->J1p[i][j] = 9999.9;
      StdI->J2[i][j] = 9999.9;
      StdI->J2p[i][j] = 9999.9;
      StdI->D[i][j] = 0.0;
    }
  }
  StdI->D[2][2] = 9999.9;
  StdI->K = 9999.9;
  StdI->L = 9999;
  StdI->Lx = 9999.9;
  StdI->Ly = 9999.9;
  StdI->mu = 9999.9;
  StdI->t = 9999.9;
  StdI->tp = 9999.9;
  StdI->t0 = 9999.9;
  StdI->t1 = 9999.9;
  StdI->t1p = 9999.9;
  StdI->t2 = 9999.9;
  StdI->t2p = 9999.9;
  StdI->U = 9999.9;
  StdI->V = 9999.9;
  StdI->Vp = 9999.9;
  StdI->V0 = 9999.9;
  StdI->V1 = 9999.9;
  StdI->V1p = 9999.9;
  StdI->V2 = 9999.9;
  StdI->V2p = 9999.9;
  StdI->W = 9999;
  StdI->Wx = 9999.9;
  StdI->Wy = 9999.9;

  strcpy(StdI->model, "****\0");
  strcpy(StdI->lattice, "****\0");
  strcpy(StdI->method, "****\0");
  strcpy(StdI->outputmode, "****\0");
  strcpy(StdI->CDataFileHead, "****\0");
  strcpy(StdI->CParaFileHead, "****\0");

  StdI->nelec = 9999;
  StdI->NVMCCalMode = 9999;
  StdI->NLanczosMode = 9999;
  StdI->NDataIdxStart = 9999;
  StdI->NDataQtySmp = 9999;
  StdI->Nsite = 9999;
  StdI->nelec = 9999;
  StdI->NSPGaussLeg = 9999;
  StdI->NSPStot = 9999;
  StdI->NMPTrans = 9999;
  StdI->NSROptItrStep = 9999;
  StdI->NSROptItrSmp = 9999;
  StdI->NSROptFixSmp = 9999;
  StdI->DSROptRedCut = 9999.9;
  StdI->DSROptStaDel = 9999.9;
  StdI->DSROptStepDt = 9999.9;
  StdI->NVMCWarmUp = 9999;
  StdI->NVMCIniterval = 9999;
  StdI->NVMCSample = 9999;
  StdI->NExUpdatePath = 9999;
  StdI->RndSeed = 9999;
}

/**
 *
 * Remove : space etc. from keyword and value in an iput file
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void TrimSpaceQuote(char *value /**< [inout] Keyword or value*/){
  char value2[256];
  int valuelen, valuelen2, ii;

  valuelen = strlen(value);
  valuelen2 = 0;
  for (ii = 0; ii < valuelen; ii++){
    if (value[ii] != ' ' &&
      value[ii] != ':' &&
      value[ii] != ';' &&
      value[ii] != '\"' &&
      value[ii] != '\b' &&
      value[ii] != '\\' &&
      value[ii] != '\v' &&
      value[ii] != '\n' &&
      value[ii] != '\0'){
      value2[valuelen2] = tolower(value[ii]);
      valuelen2++;
    }
  }

  strncpy(value, value2, valuelen2);
  value[valuelen2] = '\0';

}

/**
 *
 * Store an input value into the valiable (string)
 * If duplicated, HPhi will stop.
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void StoreWithCheckDup_s(
  char *keyword /**< [in] keyword read from the input file*/, 
  char *valuestring /**< [in] value read from the input file*/, 
  char *value /**< [out] */)
{
  if (strcmp(value, "****") != 0){
    fprintf(stdout, "ERROR !  Keyword %s is duplicated ! \n", keyword);
    exitMPI(-1);
  }
  else{
    strcpy(value, valuestring);
  }
}

/**
 *
 * Store an input value into the valiable (integer)
 * If duplicated, HPhi will stop.
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void StoreWithCheckDup_i(
  char *keyword /**< [in] keyword read from the input file*/,
  char *valuestring /**< [in] value read from the input file*/,
  int *value /**< [out] */)
{
  if (*value != 9999){
    fprintf(stdout, "ERROR !  Keyword %s is duplicated ! \n", keyword);
    exitMPI(-1);
  }
  else{
    sscanf(valuestring, "%d", value);
  }
}

/**
 *
 * Store an input value into the valiable (double)
 * If duplicated, HPhi will stop.
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void StoreWithCheckDup_d(
  char *keyword /**< [in] keyword read from the input file*/,
  char *valuestring /**< [in] value read from the input file*/,
  double *value /**< [out] */)
{

  if (*value != 9999.9){
    fprintf(stdout, "ERROR !  Keyword %s is duplicated ! \n", keyword);
    exitMPI(-1);
  }
  else{
    sscanf(valuestring, "%lf", value);
  }

}

/**
*
* Store an input value into the valiable (Double complex)
* If duplicated, HPhi will stop.
*
* @author Mitsuaki Kawamura (The University of Tokyo)
*/
static void StoreWithCheckDup_c(
  char *keyword /**< [in] keyword read from the input file*/,
  char *valuestring /**< [in] value read from the input file*/,
  double complex *value /**< [out] */)
{
  int num;
  char *valuestring_r, *valuestring_i;
  double value_r, value_i;

  if (creal(*value) != 9999.9) {
    fprintf(stdout, "ERROR !  Keyword %s is duplicated ! \n", keyword);
    exitMPI(-1);
  }
  else {

    if (valuestring[0] == ',') {
      valuestring_r = NULL;
      valuestring_i = strtok(valuestring, ",");
    }
    else {
      valuestring_r = strtok(valuestring, ",");
      valuestring_i = strtok(NULL, ",");
    }
    
    if (valuestring_r == NULL) {
      *value = 0.0;
    }
    else {
      num = sscanf(valuestring_r, "%lf", &value_r);
      if (num == 1) *value = value_r;
      else *value = 0.0;
    }

    if (valuestring_i == NULL) {
      *value += I * 0.0;
    }
    else {
        num = sscanf(valuestring_i, "%lf", &value_i);
      if (num == 1) *value += I * value_i;
      else *value += I * 0.0;
    }
  }
}

/**
*
* Print the locspin file
*
* @author Mitsuaki Kawamura (The University of Tokyo)
*/
void PrintLocSpin(struct StdIntList *StdI) {
  FILE *fp;
  int isite, nlocspin;

  nlocspin = 0;
  for (isite = 0; isite < StdI->nsite; isite++)
    if (StdI->locspinflag[isite] != 0) nlocspin = nlocspin + 1;

  fp = fopen("zlocspn.def", "w");
  fprintf(fp, "================================ \n");
  fprintf(fp, "NlocalSpin %5d  \n", nlocspin);
  fprintf(fp, "================================ \n");
  fprintf(fp, "========i_0LocSpn_1IteElc ====== \n");
  fprintf(fp, "================================ \n");

  for (isite = 0; isite < StdI->nsite; isite++)
    fprintf(fp, "%5d  %5d\n", isite, StdI->locspinflag[isite]);

  fclose(fp);
  fprintf(stdout, "    zlocspin.def is written.\n");
}

/**
*
* Print the transfer file
*
* @author Mitsuaki Kawamura (The University of Tokyo)
*/
static void PrintTrans(struct StdIntList *StdI){
  FILE *fp;
  int jtrans, ktrans, ntrans0;

  for (jtrans = 0; jtrans < StdI->ntrans; jtrans++){
    for (ktrans = jtrans + 1; ktrans < StdI->ntrans; ktrans++){
      if (StdI->transindx[jtrans][0] == StdI->transindx[ktrans][0]
        && StdI->transindx[jtrans][1] == StdI->transindx[ktrans][1]
        && StdI->transindx[jtrans][2] == StdI->transindx[ktrans][2]
        && StdI->transindx[jtrans][3] == StdI->transindx[ktrans][3]){
        StdI->trans[jtrans] = StdI->trans[jtrans] + StdI->trans[ktrans];
        StdI->trans[ktrans] = 0.0;
      }
    }/*for (ktrans = jtrans + 1; ktrans < StdI->ntrans; ktrans++)*/
  }/*for (jtrans = 0; jtrans < StdI->ntrans; jtrans++)*/

  ntrans0 = 0;
  for (ktrans = 0; ktrans < StdI->ntrans; ktrans++){
    if (cabs(StdI->trans[ktrans]) > 0.000001) ntrans0 = ntrans0 + 1;
  }

  fp = fopen("zTrans.def", "w");
  fprintf(fp, "======================== \n");
  fprintf(fp, "NTransfer %7d  \n", ntrans0);
  fprintf(fp, "======================== \n");
  fprintf(fp, "========i_j_s_tijs====== \n");
  fprintf(fp, "======================== \n");

  ntrans0 = 0;
  for (ktrans = 0; ktrans < StdI->ntrans; ktrans++) {
    if (cabs(StdI->trans[ktrans]) > 0.000001)
      fprintf(fp, "%5d %5d %5d %5d %25.15f  %25.15f\n",
        StdI->transindx[ktrans][0], StdI->transindx[ktrans][1],
        StdI->transindx[ktrans][2], StdI->transindx[ktrans][3],
        creal(StdI->trans[ktrans]), cimag(StdI->trans[ktrans]));
  }

  fclose(fp);
  fprintf(stdout, "      zTrans.def is written.\n");
}

/**
*
* Print zInterAll.def
*
* @author Mitsuaki Kawamura (The University of Tokyo)
*/
static void PrintInter(struct StdIntList *StdI){
  FILE *fp;
  int jintr, kintr, nintr0;

  for (jintr = 0; jintr < StdI->nintr; jintr++){
    for (kintr = jintr + 1; kintr < StdI->nintr; kintr++){
      if ( 
        (StdI->intrindx[jintr][0] == StdI->intrindx[kintr][0]
        && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][1]
        && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][2]
        && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][3]
        && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][4]
        && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][5]
        && StdI->intrindx[jintr][6] == StdI->intrindx[kintr][6]
        && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][7] )
        ||
        (StdI->intrindx[jintr][0] == StdI->intrindx[kintr][4]
        && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][5]
        && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][6]
        && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][7]
        && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][0]
        && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][1]
        && StdI->intrindx[jintr][6] == StdI->intrindx[kintr][2]
        && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][3])
        ){
        StdI->intr[jintr] = StdI->intr[jintr] + StdI->intr[kintr];
        StdI->intr[kintr] = 0.0;
      }
      else if (
        (StdI->intrindx[jintr][0] == StdI->intrindx[kintr][4]
        && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][5]
        && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][2]
        && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][3]
        && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][0]
        && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][1]
        && StdI->intrindx[jintr][6] == StdI->intrindx[kintr][6]
        && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][7])
        ||
        (StdI->intrindx[jintr][0] == StdI->intrindx[kintr][0]
        && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][1]
        && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][6]
        && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][7]
        && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][4]
        && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][5]
        && StdI->intrindx[jintr][6] == StdI->intrindx[kintr][2]
        && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][3])
        ){
        StdI->intr[jintr] = StdI->intr[jintr] - StdI->intr[kintr];
        StdI->intr[kintr] = 0.0;
      }
    }/*for (kintr = jintr + 1; kintr < StdI->nintr; kintr++)*/
  }/*for (jintr = 0; jintr < StdI->nintr; jintr++)*/

  for (jintr = 0; jintr < StdI->nintr; jintr++) {
    for (kintr = jintr + 1; kintr < StdI->nintr; kintr++) {
      if (StdI->intrindx[jintr][6] == StdI->intrindx[kintr][4]
        && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][5]
        && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][6]
        && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][7]
        && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][0]
        && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][1]
        && StdI->intrindx[jintr][0] == StdI->intrindx[kintr][2]
        && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][3]
        ) {
        StdI->intrindx[kintr][0] = StdI->intrindx[jintr][6];
        StdI->intrindx[kintr][1] = StdI->intrindx[jintr][7];
        StdI->intrindx[kintr][2] = StdI->intrindx[jintr][4];
        StdI->intrindx[kintr][3] = StdI->intrindx[jintr][5];
        StdI->intrindx[kintr][4] = StdI->intrindx[jintr][2];
        StdI->intrindx[kintr][5] = StdI->intrindx[jintr][3];
        StdI->intrindx[kintr][6] = StdI->intrindx[jintr][0];
        StdI->intrindx[kintr][7] = StdI->intrindx[jintr][1];
      }
      else if (
        (StdI->intrindx[jintr][6] == StdI->intrindx[kintr][4]
          && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][5]
          && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][2]
          && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][3]
          && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][0]
          && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][1]
          && StdI->intrindx[jintr][0] == StdI->intrindx[kintr][6]
          && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][7])
        ||
        (StdI->intrindx[jintr][6] == StdI->intrindx[kintr][0]
          && StdI->intrindx[jintr][7] == StdI->intrindx[kintr][1]
          && StdI->intrindx[jintr][4] == StdI->intrindx[kintr][6]
          && StdI->intrindx[jintr][5] == StdI->intrindx[kintr][7]
          && StdI->intrindx[jintr][2] == StdI->intrindx[kintr][4]
          && StdI->intrindx[jintr][3] == StdI->intrindx[kintr][5]
          && StdI->intrindx[jintr][0] == StdI->intrindx[kintr][2]
          && StdI->intrindx[jintr][1] == StdI->intrindx[kintr][3])
        ) {
        StdI->intrindx[kintr][0] = StdI->intrindx[jintr][6];
        StdI->intrindx[kintr][1] = StdI->intrindx[jintr][7];
        StdI->intrindx[kintr][2] = StdI->intrindx[jintr][4];
        StdI->intrindx[kintr][3] = StdI->intrindx[jintr][5];
        StdI->intrindx[kintr][4] = StdI->intrindx[jintr][2];
        StdI->intrindx[kintr][5] = StdI->intrindx[jintr][3];
        StdI->intrindx[kintr][6] = StdI->intrindx[jintr][0];
        StdI->intrindx[kintr][7] = StdI->intrindx[jintr][1];

        StdI->intr[kintr] = -StdI->intr[kintr];
      }
    }/*for (kintr = jintr + 1; kintr < StdI->nintr; kintr++)*/
  }/*for (jintr = 0; jintr < StdI->nintr; jintr++)*/

  for (jintr = 0; jintr < StdI->nintr; jintr++) {

    if (
      (StdI->intrindx[jintr][0] == StdI->intrindx[jintr][4] 
        && StdI->intrindx[jintr][1] == StdI->intrindx[jintr][5]) ||
      (StdI->intrindx[jintr][2] == StdI->intrindx[jintr][6] 
        && StdI->intrindx[jintr][3] == StdI->intrindx[jintr][7])
      ) {

      if (!(
        (StdI->intrindx[jintr][0] == StdI->intrindx[jintr][2] 
          && StdI->intrindx[jintr][1] == StdI->intrindx[jintr][3])
        ||
        (StdI->intrindx[jintr][0] == StdI->intrindx[jintr][6]
          && StdI->intrindx[jintr][1] == StdI->intrindx[jintr][7]) 
        ||
        (StdI->intrindx[jintr][4] == StdI->intrindx[jintr][2] 
          && StdI->intrindx[jintr][5] == StdI->intrindx[jintr][3])
        ||
        (StdI->intrindx[jintr][4] == StdI->intrindx[jintr][6] 
          && StdI->intrindx[jintr][5] == StdI->intrindx[jintr][7])
        ))
        StdI->intr[jintr] = 0.0;
    }
  }/*for (jintr = 0; jintr < StdI->nintr; jintr++)*/
 
  nintr0 = 0;
  for (kintr = 0; kintr < StdI->nintr; kintr++){
    if (cabs(StdI->intr[kintr]) > 0.000001) nintr0 = nintr0 + 1;
  }

  fp = fopen("zInterAll.def", "w");
  fprintf(fp, "====================== \n");
  fprintf(fp, "NInterAll %7d  \n", nintr0);
  fprintf(fp, "====================== \n");
  fprintf(fp, "========zInterAll===== \n");
  fprintf(fp, "====================== \n");

  nintr0 = 0;
  for (kintr = 0; kintr < StdI->nintr; kintr++) {
    if (cabs(StdI->intr[kintr]) > 0.000001)
      fprintf(fp, "%5d %5d %5d %5d %5d %5d %5d %5d %25.15f  %25.15f\n",
        StdI->intrindx[kintr][0], StdI->intrindx[kintr][1],
        StdI->intrindx[kintr][2], StdI->intrindx[kintr][3],
        StdI->intrindx[kintr][4], StdI->intrindx[kintr][5],
        StdI->intrindx[kintr][6], StdI->intrindx[kintr][7],
        creal(StdI->intr[kintr]), cimag(StdI->intr[kintr]));
  }/*for (kintr = 0; kintr < StdI->nintr; kintr++)*/

  fclose(fp);
  fprintf(stdout, "   zInterAll.def is written.\n");
}

/**
 *
 * Print namelist.def  
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void PrintNamelist(struct StdIntList *StdI){
  FILE *fp;

  fp = fopen("namelist.def", "w");
  fprintf(fp, "CalcMod calcmod.def\n");
  fprintf(fp, "ModPara modpara.def\n");
  fprintf(fp, "LocSpin zlocspn.def\n");
  fprintf(fp, "Trans zTrans.def\n");
  fprintf(fp, "InterAll zInterAll.def\n");
  fprintf(fp, "OneBodyG greenone.def\n");
  fprintf(fp, "TwoBodyG greentwo.def\n");

  fclose(fp);
  fprintf(stdout, "    namelist.def is written.\n");
}

/**
 *
 * Print calcmod.def
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void PrintCalcMod(struct StdIntList *StdI)
{
  FILE *fp;
  int iCalcType, iCalcModel, ioutputmode2;

  if (strcmp(StdI->method, "****") == 0){
    fprintf(stdout, "ERROR ! Method is NOT specified !\n");
    exitMPI(-1);
  }
  else if (strcmp(StdI->method, "lanczos") == 0) iCalcType = 0;
  else if (strcmp(StdI->method, "tpq") == 0) iCalcType = 1;
  else if (strcmp(StdI->method, "fulldiag") == 0 ||
    strcmp(StdI->method, "alldiag") == 0 ||
    strcmp(StdI->method, "direct") == 0 ) iCalcType = 2;
  else{
    fprintf(stdout, "\n ERROR ! Unsupported Solver : %s\n", StdI->method);
    exitMPI(-1);
  }

  if (strcmp(StdI->model, "hubbard") == 0) {
    if (StdI->lGC == 0)iCalcModel = 0;
    else iCalcModel = 3;
  }
  else if (strcmp(StdI->model, "spin") == 0) {
    if (StdI->lGC == 0)iCalcModel = 1;
    else iCalcModel = 4;
  }
  if (strcmp(StdI->model, "kondo") == 0) {
    if (StdI->lGC == 0)iCalcModel = 2;
    else iCalcModel = 5;
  }

  if (StdI->ioutputmode == 2) ioutputmode2 = 0;
  else ioutputmode2 = StdI->ioutputmode;

  fp = fopen("calcmod.def", "w");
  fprintf(fp, "#CalcType = 0:Lanczos, 1:TPQCalc, 2:FullDiag\n");
  fprintf(fp, "#FlgFiniteTemperature= 0:Zero temperature, 1:Finite temperature. This parameter is active only for CalcType=2.\n");
  fprintf(fp, "#CalcModel = 0:Hubbard, 1:Spin, 2:Kondo, 3:HubbardGC, 4:SpinGC, 5:KondoGC \n");
  fprintf(fp, "CalcType %3d\n", iCalcType);
  fprintf(fp, "FlgFiniteTemperature %3d\n", StdI->FlgTemp);
  fprintf(fp, "CalcModel %3d\n", iCalcModel);
  fprintf(fp, "OutputMode %3d\n", ioutputmode2);
  fclose(fp);
  fprintf(stdout, "     calcmod.def is written.\n");
}

/**
 *
 * Print modpara.def
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void PrintModPara(struct StdIntList *StdI)
{
  FILE *fp;

  fp = fopen("modpara.def", "w");
  fprintf(fp, "--------------------\n");
  fprintf(fp, "Model_Parameters   0\n");
  fprintf(fp, "--------------------\n");
  fprintf(fp, "VMC_Cal_Parameters\n");
  fprintf(fp, "--------------------\n");
  fprintf(fp, "CDataFileHead  %s\n", StdI->CDataFileHead);
  fprintf(fp, "CParaFileHead  %s\n", StdI->CParaFileHead);
  fprintf(fp, "--------------------\n");
  fprintf(fp, "NVMCCalMode    %-5d\n", StdI->NVMCCalMode);
  fprintf(fp, "Nsite          %-5d\n", StdI->nsite);

  fclose(fp);
  fprintf(stdout, "     modpara.def is written.\n");
}

/**
 *
 * Print greenone.def
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void Print1Green(struct StdIntList *StdI)
{
  FILE *fp;
  int ngreen, igreen, isite, jsite, ispin,jspin, SiMax, SjMax;
  int **greenindx;

  if (StdI->ioutputmode == 0){
    ngreen = 0;
  }
  else if(StdI->ioutputmode == 1){
    ngreen = 0;
    for (isite = 0; isite < StdI->nsite; isite++){
      if (StdI->locspinflag[isite] == 0) ngreen = ngreen + 2;
      else ngreen = ngreen + (StdI->locspinflag[isite] + 1);
    }
  }
  else{
    ngreen = 0;
    for (isite = 0; isite < StdI->nsite; isite++){
      if (StdI->locspinflag[isite] == 0) ngreen = ngreen + 2;
      else ngreen = ngreen + (StdI->locspinflag[isite] + 1);
    }
    ngreen = ngreen * ngreen;
  }
  greenindx = (int **)malloc(sizeof(int*) * (ngreen + 1));
  for (igreen = 0; igreen < ngreen; igreen++){
    greenindx[igreen] = (int *)malloc(sizeof(int) * 4);
  }
  if (StdI->ioutputmode == 1){
    igreen = 0;
    for (isite = 0; isite < StdI->nsite; isite++){

      if (StdI->locspinflag[isite] == 0) SiMax = 1;
      else SiMax = StdI->locspinflag[isite];
      
      for (ispin = 0; ispin <= SiMax; ispin++){
        greenindx[igreen][0] = isite;
        greenindx[igreen][1] = ispin;
        greenindx[igreen][2] = isite;
        greenindx[igreen][3] = ispin;
        igreen++;
      }
    }
  }
  else if (StdI->ioutputmode == 2){
    igreen = 0;
    for (isite = 0; isite < StdI->nsite; isite++){

      if (StdI->locspinflag[isite] == 0) SiMax = 1;
      else SiMax = StdI->locspinflag[isite];

      for (ispin = 0; ispin <= SiMax; ispin++){
        for (jsite = 0; jsite < StdI->nsite; jsite++){

          if (StdI->locspinflag[jsite] == 0) SjMax = 1;
          else SjMax = StdI->locspinflag[jsite];

          for (jspin = 0; jspin <= SjMax; jspin++){
            greenindx[igreen][0] = isite;
            greenindx[igreen][1] = ispin;
            greenindx[igreen][2] = jsite;
            greenindx[igreen][3] = jspin;
            igreen++;
          }
        }
      }
    }
  }

  fp = fopen("greenone.def", "w");
  fprintf(fp, "===============================\n");
  fprintf(fp, "NCisAjs %10d\n", ngreen);
  fprintf(fp, "===============================\n");
  fprintf(fp, "======== Green functions ======\n");
  fprintf(fp, "===============================\n");
  for (igreen = 0; igreen < ngreen; igreen++){
    fprintf(fp,"%5d %5d %5d %5d\n",
      greenindx[igreen][0], greenindx[igreen][1], greenindx[igreen][2], greenindx[igreen][3]);
  }
  fclose(fp);

  fprintf(stdout, "    greenone.def is written.\n");
  //[s] free
  for (igreen = 0; igreen < ngreen; igreen++){
    free(greenindx[igreen]);
  }
  free(greenindx);
  //[e] free
}

/**
 *
 * Print greentwo.def
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void Print2Green(struct StdIntList *StdI){
  FILE *fp;
  int ngreen, igreen;
  int site1, site2, site3, site4;
  int spin1, spin2, spin3, spin4;
  int S1Max, S2Max, S3Max, S4Max;
  int **greenindx;

  if (StdI->ioutputmode == 0){
    ngreen = 0;
  }
  else if (StdI->ioutputmode == 1){
    ngreen = 0;
    for (site1 = 0; site1 < StdI->nsite; site1++){
      if (StdI->locspinflag[site1] == 0) ngreen = ngreen + 2;
      else ngreen = ngreen + (StdI->locspinflag[site1] + 1);
    }
    ngreen = ngreen * ngreen;
  }
  else{
    ngreen = 0;
    for (site1 = 0; site1 < StdI->nsite; site1++){
      if (StdI->locspinflag[site1] == 0) ngreen = ngreen + 2;
      else ngreen = ngreen + (StdI->locspinflag[site1] + 1);
    }
    ngreen = ngreen * ngreen * ngreen * ngreen;
  }
  greenindx = (int **)malloc(sizeof(int*) * (ngreen + 1));
  for (igreen = 0; igreen < ngreen; igreen++){
    greenindx[igreen] = (int *)malloc(sizeof(int) * 8);
  }
  if (StdI->ioutputmode == 1){
    igreen = 0;
    for (site1 = 0; site1 < StdI->nsite; site1++){

      if (StdI->locspinflag[site1] == 0) S1Max = 1;
      else S1Max = StdI->locspinflag[site1];

      for (spin1 = 0; spin1 <= S1Max; spin1++){
        for (site2 = 0; site2 < StdI->nsite; site2++){

          if (StdI->locspinflag[site2] == 0) S2Max = 1;
          else S2Max = StdI->locspinflag[site2];

          for (spin2 = 0; spin2 <= S2Max; spin2++){
            greenindx[igreen][0] = site1;
            greenindx[igreen][1] = spin1;
            greenindx[igreen][2] = site1;
            greenindx[igreen][3] = spin1;
            greenindx[igreen][4] = site2;
            greenindx[igreen][5] = spin2;
            greenindx[igreen][6] = site2;
            greenindx[igreen][7] = spin2;
           igreen++;
          }
        }
      }
    }
   }
  else if (StdI->ioutputmode == 2){
    igreen = 0;
    for (site1 = 0; site1 < StdI->nsite; site1++){

      if (StdI->locspinflag[site1] == 0) S1Max = 1;
      else S1Max = StdI->locspinflag[site1];

      for (spin1 = 0; spin1 <= S1Max; spin1++){
        for (site2 = 0; site2 < StdI->nsite; site2++){

          if (StdI->locspinflag[site1] != 0 && StdI->locspinflag[site2] != 0 
            && site1 != site2) continue;

          if (StdI->locspinflag[site2] == 0) S2Max = 1;
          else S2Max = StdI->locspinflag[site2];

          for (spin2 = 0; spin2 <= S2Max; spin2++){
            for (site3 = 0; site3 < StdI->nsite; site3++){

              if (StdI->locspinflag[site3] == 0) S3Max = 1;
              else S3Max = StdI->locspinflag[site3];

              for (spin3 = 0; spin3 <= S3Max; spin3++){
                for (site4 = 0; site4 < StdI->nsite; site4++){

                  if (StdI->locspinflag[site3] != 0 && StdI->locspinflag[site4] != 0
                    && site3 != site4) continue;

                  if (StdI->locspinflag[site4] == 0) S4Max = 1;
                  else S4Max = StdI->locspinflag[site4];

                  for (spin4 = 0; spin4 <= S4Max; spin4++){
                    greenindx[igreen][0] = site1;
                    greenindx[igreen][1] = spin1;
                    greenindx[igreen][2] = site2;
                    greenindx[igreen][3] = spin2;
                    greenindx[igreen][4] = site3;
                    greenindx[igreen][5] = spin3;
                    greenindx[igreen][6] = site4;
                    greenindx[igreen][7] = spin4;
                    igreen++;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  ngreen = igreen;

  fp = fopen("greentwo.def", "w");
  fprintf(fp, "=============================================\n");
  fprintf(fp, "NCisAjsCktAltDC %10d\n", ngreen);
  fprintf(fp, "=============================================\n");
  fprintf(fp, "======== Green functions for Sq AND Nq ======\n");
  fprintf(fp, "=============================================\n");
  for (igreen = 0; igreen < ngreen; igreen++){
    fprintf(fp,"%5d %5d %5d %5d %5d %5d %5d %5d\n",
      greenindx[igreen][0], greenindx[igreen][1], greenindx[igreen][2], greenindx[igreen][3],
      greenindx[igreen][4], greenindx[igreen][5], greenindx[igreen][6], greenindx[igreen][7]);
  }
  fclose(fp);

  fprintf(stdout, "    greentwo.def is written.\n");
  //[s] free
  for (igreen = 0; igreen < ngreen; igreen++){
    free(greenindx[igreen]);
  }
  free(greenindx);
  //[e] free
}

/**
 *
 * Stop HPhi if unsupported model is read 
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void UnsupportedSystem(
  char *model /**< [in]*/, 
  char *lattice /**< [in]*/)
{
  fprintf(stdout, "\nSorry, specified combination, \n");
  fprintf(stdout, "    MODEL : %s  \n", model);
  fprintf(stdout, "  LATTICE : %s, \n", lattice);
  fprintf(stdout, "is unsupported in the STANDARD MODE...\n");
  fprintf(stdout, "Please use the EXPART MODE, or write a NEW FUNCTION and post us.\n");
  exitMPI(-1);
}

/**
 *
 * Verify outputmode
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void CheckOutputMode(struct StdIntList *StdI)
{
  /*
  Form for Correlation function
  */
  if (strcmp(StdI->outputmode, "non") == 0
    || strcmp(StdI->outputmode, "none") == 0
    || strcmp(StdI->outputmode, "off") == 0) {
    StdI->ioutputmode = 0;
    fprintf(stdout, "      ioutputmode = %-10d\n", StdI->ioutputmode);
  }
  else if (strcmp(StdI->outputmode, "cor") == 0
    || strcmp(StdI->outputmode, "corr") == 0
    || strcmp(StdI->outputmode, "correlation") == 0) {
    StdI->ioutputmode = 1;
    fprintf(stdout, "      ioutputmode = %-10d\n", StdI->ioutputmode);
  }
  else if (strcmp(StdI->outputmode, "****") == 0) {
    StdI->ioutputmode = 1;
    fprintf(stdout, "      ioutputmode = %-10d  ######  DEFAULT VALUE IS USED  ######\n", StdI->ioutputmode);
  }
  else if (strcmp(StdI->outputmode, "raw") == 0
    || strcmp(StdI->outputmode, "all") == 0
    || strcmp(StdI->outputmode, "full") == 0) {
    StdI->ioutputmode = 2;
    fprintf(stdout, "      ioutputmode = %-10d\n", StdI->ioutputmode);
  }
  else{
    fprintf(stdout, "\n ERROR ! Unsupported OutPutMode : %s\n", StdI->outputmode);
    exitMPI(-1);
  }
}

/**
 *
 * Summary numerical parameter check the combination of
 * the number of sites, total spin, the number of electrons
 *
 * @author Mitsuaki Kawamura (The University of Tokyo)
 */
static void CheckModPara(struct StdIntList *StdI)
{
  if (strcmp(StdI->CDataFileHead, "****") == 0) {
    strcpy(StdI->CDataFileHead, "zvo\0");
    fprintf(stdout, "         filehead = %-12s######  DEFAULT VALUE IS USED  ######\n", StdI->CDataFileHead);
  }
  else fprintf(stdout, "         filehead = %-s\n", StdI->CDataFileHead);
 
  if (strcmp(StdI->CParaFileHead, "****") == 0) {
    strcpy(StdI->CParaFileHead, "zqp\0");
    fprintf(stdout, "         filehead = %-12s######  DEFAULT VALUE IS USED  ######\n", StdI->CParaFileHead);
  }
  else fprintf(stdout, "         filehead = %-s\n", StdI->CParaFileHead);

  /**/
  StdFace_PrintVal_i("NVMCCalMode", &StdI->NVMCCalMode, 0);
  StdFace_PrintVal_i("NLanczosMode", &StdI->NLanczosMode, 0);
  StdFace_PrintVal_i("NDataIdxStart", &StdI->NDataIdxStart, 1);

  if (StdI->NVMCCalMode == 0) StdFace_NotUsed_i("NDataQtySmp", StdI->NDataQtySmp);
  else StdFace_PrintVal_i("NDataQtySmp", &StdI->NDataQtySmp, 5);

  StdFace_PrintVal_i("NSPGaussLeg", &StdI->NSPGaussLeg, 1);
  StdFace_PrintVal_i("NSPStot", &StdI->NSPStot, 0);
  StdFace_PrintVal_i("NMPTrans", &StdI->NMPTrans, 1);

  if (StdI->NVMCCalMode == 1) StdFace_NotUsed_i("NSROptItrStep", StdI->NSROptItrStep);
  else StdFace_PrintVal_i("NSROptItrStep", &StdI->NSROptItrStep, 1200);
  
  if (StdI->NVMCCalMode == 1) StdFace_NotUsed_i("NSROptItrSmp", StdI->NSROptItrSmp);
  else StdFace_PrintVal_i("NSROptItrSmp", &StdI->NSROptItrSmp, 100);

  StdFace_PrintVal_i("NSROptFixSmp", &StdI->NSROptFixSmp, 1);
  StdFace_PrintVal_i("NVMCWarmUp", &StdI->NVMCWarmUp, 10);
  StdFace_PrintVal_i("NVMCIniterval", &StdI->NVMCIniterval, 1);
  StdFace_PrintVal_i("NVMCSample", &StdI->NVMCSample, 100);
  StdFace_PrintVal_i("NExUpdatePath", &StdI->NExUpdatePath, 0);
  StdFace_PrintVal_i("RndSeed", &StdI->RndSeed, 123456789);

  StdFace_PrintVal_d("DSROptRedCut", &StdI->DSROptRedCut, 0.0001);
  StdFace_PrintVal_d("DSROptStaDel", &StdI->DSROptStaDel, 0.01);
  StdFace_PrintVal_d("DSROptStepDt", &StdI->DSROptStepDt, 0.05);

  /**/
  if (strcmp(StdI->model, "hubbard") == 0){
    if (StdI->lGC == 0) StdFace_RequiredVal_i("nelec", StdI->nelec);
    else {
      StdFace_NotUsed_i("nelec", StdI->nelec);
      StdFace_NotUsed_i("2Sz", StdI->Sz2);
    }
  }
  else if (strcmp(StdI->model, "spin") == 0) {
    StdFace_NotUsed_i("nelec", StdI->nelec);
    if (StdI->lGC == 0) StdFace_RequiredVal_i("2Sz", StdI->Sz2);
    else StdFace_NotUsed_i("2Sz", StdI->Sz2);
  }
  else if (strcmp(StdI->model, "kondo") == 0) {
    if (StdI->lGC == 0) StdFace_RequiredVal_i("nelec", StdI->nelec);
    else {
      StdFace_NotUsed_i("nelec", StdI->nelec);
      StdFace_NotUsed_i("2Sz", StdI->Sz2);
    }
  }
}

/**
*
* Main routine for the standard mode
*
* @author Mitsuaki Kawamura (The University of Tokyo)
*/
void StdFace_main(char *fname  /**< [in] Input file name for the standard mode */) {

  struct StdIntList StdI;

  FILE *fp;
  int ktrans, kintr;
  char ctmpline[256];
  char *keyword, *value;

  fprintf(stdout, "\n######  Standard Intarface Mode STARTS  ######\n");
  if ((fp = fopen(fname, "r")) == NULL) {
    fprintf(stdout, "\n  ERROR !  Cannot open input file %s !\n\n", fname);
    exitMPI(-1);
  }
  else {
    fprintf(stdout, "\n  Open Standard-Mode Inputfile %s \n\n", fname);
  }

  StdFace_ResetVals(&StdI);

  while (fgets(ctmpline, 256, fp) != NULL) {

    TrimSpaceQuote(ctmpline);
    if (strncmp(ctmpline, "//", 2) == 0) {
      fprintf(stdout, "  Skipping a line.\n");
      continue;
    }
    else if (ctmpline[0] == '\0') {
      fprintf(stdout, "  Skipping a line.\n");
      continue;
    }
    keyword = strtok(ctmpline, "=");
    value = strtok(NULL, "=");
    if (value == NULL) {
      fprintf(stdout, "\n  ERROR !  \"=\" is NOT found !\n\n");
      exitMPI(-1);
    }
    TrimSpaceQuote(keyword);
    TrimSpaceQuote(value);
    fprintf(stdout, "  KEYWORD : %-20s | VALUE : %s \n", keyword, value);

    if (strcmp(keyword, "a") == 0) StoreWithCheckDup_d(keyword, value, &StdI.a);
    else if (strcmp(keyword, "a0") == 0) StoreWithCheckDup_d(keyword, value, &StdI.a0);
    else if (strcmp(keyword, "a0l") == 0) StoreWithCheckDup_i(keyword, value, &StdI.a0L);
    else if (strcmp(keyword, "a0w") == 0) StoreWithCheckDup_i(keyword, value, &StdI.a0W);
    else if (strcmp(keyword, "a1") == 0) StoreWithCheckDup_d(keyword, value, &StdI.a1);
    else if (strcmp(keyword, "a1l") == 0) StoreWithCheckDup_i(keyword, value, &StdI.a1L);
    else if (strcmp(keyword, "a1w") == 0) StoreWithCheckDup_i(keyword, value, &StdI.a1W);
    else if (strcmp(keyword, "d") == 0) StoreWithCheckDup_d(keyword, value, &StdI.D[2][2]);
    else if (strcmp(keyword, "dsroptredcut") == 0) StoreWithCheckDup_d(keyword, value, &StdI.DSROptRedCut);
    else if (strcmp(keyword, "dsroptstadel") == 0) StoreWithCheckDup_d(keyword, value, &StdI.DSROptStaDel);
    else if (strcmp(keyword, "dsroptstepdt") == 0) StoreWithCheckDup_d(keyword, value, &StdI.DSROptStepDt);
    else if (strcmp(keyword, "gamma") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Gamma);
    else if (strcmp(keyword, "h") == 0) StoreWithCheckDup_d(keyword, value, &StdI.h);
    else if (strcmp(keyword, "j") == 0) StoreWithCheckDup_d(keyword, value, &StdI.JAll);
    else if (strcmp(keyword, "jastrowaniso") == 0) StoreWithCheckDup_i(keyword, value, &StdI.JastrowAniso);
    else if (strcmp(keyword, "jastrowcut") == 0) StoreWithCheckDup_i(keyword, value, &StdI.JastrowCut);
    else if (strcmp(keyword, "jx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[0][0]);
    else if (strcmp(keyword, "jxy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[0][1]);
    else if (strcmp(keyword, "jxz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[0][2]);
    else if (strcmp(keyword, "jy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[1][1]);
    else if (strcmp(keyword, "jyx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[1][0]);
    else if (strcmp(keyword, "jyz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[1][2]);
    else if (strcmp(keyword, "jz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[2][2]);
    else if (strcmp(keyword, "jzx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[2][0]);
    else if (strcmp(keyword, "jzy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J[2][1]);
    else if (strcmp(keyword, "j0") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0All);
    else if (strcmp(keyword, "j0x") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[0][0]);
    else if (strcmp(keyword, "j0xy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[0][1]);
    else if (strcmp(keyword, "j0xz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[0][2]);
    else if (strcmp(keyword, "j0y") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[1][1]);
    else if (strcmp(keyword, "j0yx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[1][0]);
    else if (strcmp(keyword, "j0yz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[1][2]);
    else if (strcmp(keyword, "j0z") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[2][2]);
    else if (strcmp(keyword, "j0zx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[2][0]);
    else if (strcmp(keyword, "j0zy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J0[2][1]);
    else if (strcmp(keyword, "j1") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1All);
    else if (strcmp(keyword, "j1x") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[0][0]);
    else if (strcmp(keyword, "j1xy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[0][1]);
    else if (strcmp(keyword, "j1xz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[0][2]);
    else if (strcmp(keyword, "j1y") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[1][1]);
    else if (strcmp(keyword, "j1yx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[1][0]);
    else if (strcmp(keyword, "j1yz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[1][2]);
    else if (strcmp(keyword, "j1z") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[2][2]);
    else if (strcmp(keyword, "j1zx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[2][0]);
    else if (strcmp(keyword, "j1zy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1[2][1]);
    else if (strcmp(keyword, "j1'") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1pAll);
    else if (strcmp(keyword, "j1'x") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[0][0]);
    else if (strcmp(keyword, "j1'xy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[0][1]);
    else if (strcmp(keyword, "j1'xz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[0][2]);
    else if (strcmp(keyword, "j1'y") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[1][1]);
    else if (strcmp(keyword, "j1'yx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[1][0]);
    else if (strcmp(keyword, "j1'yz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[1][2]);
    else if (strcmp(keyword, "j1'z") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[2][2]);
    else if (strcmp(keyword, "j1'zx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[2][0]);
    else if (strcmp(keyword, "j1'zy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J1p[2][1]);
    else if (strcmp(keyword, "j2") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2All);
    else if (strcmp(keyword, "j2x") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[0][0]);
    else if (strcmp(keyword, "j2xy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[0][1]);
    else if (strcmp(keyword, "j2xz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[0][2]);
    else if (strcmp(keyword, "j2y") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[1][1]);
    else if (strcmp(keyword, "j2yx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[1][0]);
    else if (strcmp(keyword, "j2yz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[1][2]);
    else if (strcmp(keyword, "j2z") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[2][2]);
    else if (strcmp(keyword, "j2zx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[2][0]);
    else if (strcmp(keyword, "j2zy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2[2][1]);
    else if (strcmp(keyword, "j2'") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2pAll);
    else if (strcmp(keyword, "j2'x") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[0][0]);
    else if (strcmp(keyword, "j2'xy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[0][1]);
    else if (strcmp(keyword, "j2'xz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[0][2]);
    else if (strcmp(keyword, "j2'y") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[1][1]);
    else if (strcmp(keyword, "j2'yx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[1][0]);
    else if (strcmp(keyword, "j2'yz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[1][2]);
    else if (strcmp(keyword, "j2'z") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[2][2]);
    else if (strcmp(keyword, "j2'zx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[2][0]);
    else if (strcmp(keyword, "j2'zy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.J2p[2][1]);
    else if (strcmp(keyword, "j'") == 0) StoreWithCheckDup_d(keyword, value, &StdI.JpAll);
    else if (strcmp(keyword, "j'x") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[0][0]);
    else if (strcmp(keyword, "j'xy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[0][1]);
    else if (strcmp(keyword, "j'xz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[0][2]);
    else if (strcmp(keyword, "j'y") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[1][1]);
    else if (strcmp(keyword, "j'yx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[1][0]);
    else if (strcmp(keyword, "j'yz") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[1][2]);
    else if (strcmp(keyword, "j'z") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[2][2]);
    else if (strcmp(keyword, "j'zx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[2][0]);
    else if (strcmp(keyword, "j'zy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Jp[2][1]);
    else if (strcmp(keyword, "k") == 0) StoreWithCheckDup_d(keyword, value, &StdI.K);
    else if (strcmp(keyword, "l") == 0) StoreWithCheckDup_i(keyword, value, &StdI.L);
    else if (strcmp(keyword, "lattice") == 0) StoreWithCheckDup_s(keyword, value, StdI.lattice);
    else if (strcmp(keyword, "lx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Lx);
    else if (strcmp(keyword, "ly") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Ly);
    else if (strcmp(keyword, "method") == 0) StoreWithCheckDup_s(keyword, value, StdI.method);
    else if (strcmp(keyword, "model") == 0) StoreWithCheckDup_s(keyword, value, StdI.model);
    else if (strcmp(keyword, "mu") == 0) StoreWithCheckDup_d(keyword, value, &StdI.mu);
    else if (strcmp(keyword, "nvmccalmode") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NVMCCalMode);
    else if (strcmp(keyword, "nelec") == 0) StoreWithCheckDup_i(keyword, value, &StdI.nelec);
    else if (strcmp(keyword, "ndataidxstart") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NDataIdxStart);
    else if (strcmp(keyword, "ndataqtysmp") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NDataQtySmp);
    else if (strcmp(keyword, "nexupdatepath") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NExUpdatePath);
    else if (strcmp(keyword, "nlanczosmode") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NLanczosMode);
    else if (strcmp(keyword, "nmptrans") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NMPTrans);
    else if (strcmp(keyword, "nspgaussLeg") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NSPGaussLeg);
    else if (strcmp(keyword, "nsroptfixsmp") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NSROptFixSmp);
    else if (strcmp(keyword, "nsroptitrsmp") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NSROptItrSmp);
    else if (strcmp(keyword, "nsroptitrstep") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NSROptItrStep);
    else if (strcmp(keyword, "nvmciniterval") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NVMCIniterval);
    else if (strcmp(keyword, "nvmcsample") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NVMCSample);
    else if (strcmp(keyword, "nvmcwarmup") == 0) StoreWithCheckDup_i(keyword, value, &StdI.NVMCWarmUp);
    else if (strcmp(keyword, "outputmode") == 0) StoreWithCheckDup_s(keyword, value, StdI.outputmode);
    else if (strcmp(keyword, "rndseed") == 0) StoreWithCheckDup_i(keyword, value, &StdI.RndSeed);
    else if (strcmp(keyword, "sub0l") == 0) StoreWithCheckDup_i(keyword, value, &StdI.sub0L);
    else if (strcmp(keyword, "sub0w") == 0) StoreWithCheckDup_i(keyword, value, &StdI.sub0W);
    else if (strcmp(keyword, "sub1l") == 0) StoreWithCheckDup_i(keyword, value, &StdI.sub1L);
    else if (strcmp(keyword, "sub1w") == 0) StoreWithCheckDup_i(keyword, value, &StdI.sub1W);
    else if (strcmp(keyword, "t") == 0) StoreWithCheckDup_c(keyword, value, &StdI.t);
    else if (strcmp(keyword, "t0") == 0) StoreWithCheckDup_c(keyword, value, &StdI.t0);
    else if (strcmp(keyword, "t1") == 0) StoreWithCheckDup_c(keyword, value, &StdI.t1);
    else if (strcmp(keyword, "t1'") == 0) StoreWithCheckDup_c(keyword, value, &StdI.t1p);
    else if (strcmp(keyword, "t2") == 0) StoreWithCheckDup_c(keyword, value, &StdI.t2);
    else if (strcmp(keyword, "t2'") == 0) StoreWithCheckDup_c(keyword, value, &StdI.t2p);
    else if (strcmp(keyword, "t'") == 0) StoreWithCheckDup_c(keyword, value, &StdI.tp);
    else if (strcmp(keyword, "u") == 0) StoreWithCheckDup_d(keyword, value, &StdI.U);
    else if (strcmp(keyword, "v") == 0) StoreWithCheckDup_d(keyword, value, &StdI.V);
    else if (strcmp(keyword, "v0") == 0) StoreWithCheckDup_d(keyword, value, &StdI.V0);
    else if (strcmp(keyword, "v1") == 0) StoreWithCheckDup_d(keyword, value, &StdI.V1);
    else if (strcmp(keyword, "v1'") == 0) StoreWithCheckDup_d(keyword, value, &StdI.V1p);
    else if (strcmp(keyword, "v2") == 0) StoreWithCheckDup_d(keyword, value, &StdI.V2);
    else if (strcmp(keyword, "v2p") == 0) StoreWithCheckDup_d(keyword, value, &StdI.V2);
    else if (strcmp(keyword, "v'") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Vp);
    else if (strcmp(keyword, "w") == 0) StoreWithCheckDup_i(keyword, value, &StdI.W);
    else if (strcmp(keyword, "wx") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Wx);
    else if (strcmp(keyword, "wy") == 0) StoreWithCheckDup_d(keyword, value, &StdI.Wy);
    else {
      fprintf(stdout, "ERROR ! Unsupported Keyword !\n");
      exitMPI(-1);
    }
  }
  fclose(fp);
  /*
  Check the model
  */
  StdI.lGC = 0;
  if (strcmp(StdI.model, "fermionhubbard") == 0
    || strcmp(StdI.model, "hubbard") == 0)
    strcpy(StdI.model, "hubbard\0");
  else if(strcmp(StdI.model, "fermionhubbardgc") == 0
    || strcmp(StdI.model, "hubbardgc") == 0) {
    strcpy(StdI.model, "hubbard\0");
    StdI.lGC = 1;
  }
  else if (strcmp(StdI.model, "spin") == 0)
    strcpy(StdI.model, "spin\0");
  else if (strcmp(StdI.model, "spingc") == 0) {
    strcpy(StdI.model, "spin\0");
    StdI.lGC = 1;
  }
  else if (strcmp(StdI.model, "kondolattice") == 0
    || strcmp(StdI.model, "kondo") == 0) {
    strcpy(StdI.model, "kondo\0");
  }
  else if(strcmp(StdI.model, "kondolatticegc") == 0
    || strcmp(StdI.model, "kondogc") == 0) {
    strcpy(StdI.model, "kondo\0");
    StdI.lGC = 1;
  }
  else UnsupportedSystem(StdI.model, StdI.lattice);
  /*
  Generate Hamiltonian definition files
  */
  if (strcmp(StdI.lattice, "chain") == 0
    || strcmp(StdI.lattice, "chainlattice") == 0) StdFace_Chain(&StdI, StdI.model);
  else if (strcmp(StdI.lattice, "honeycomb") == 0
    || strcmp(StdI.lattice, "honeycomblattice") == 0) StdFace_Honeycomb(&StdI, StdI.model);
  else if (strcmp(StdI.lattice, "kagome") == 0
    || strcmp(StdI.lattice, "kagomelattice") == 0) StdFace_Kagome(&StdI, StdI.model);
  else if (strcmp(StdI.lattice, "ladder") == 0
    || strcmp(StdI.lattice, "ladderlattice") == 0) StdFace_Ladder(&StdI, StdI.model);
  else if (strcmp(StdI.lattice, "tetragonal") == 0
    || strcmp(StdI.lattice, "tetragonallattice") == 0
    || strcmp(StdI.lattice, "square") == 0
    || strcmp(StdI.lattice, "squarelattice") == 0) StdFace_Tetragonal(&StdI, StdI.model);
  else if (strcmp(StdI.lattice, "triangular") == 0
    || strcmp(StdI.lattice, "triangularlattice") == 0) StdFace_Triangular(&StdI, StdI.model);
  else UnsupportedSystem(StdI.model, StdI.lattice);
  /**/
  CheckModPara(&StdI);
  CheckOutputMode(&StdI);
  /**/
  fprintf(stdout, "\n");
  fprintf(stdout, "######  Print Expert input files  ######\n");
  fprintf(stdout, "\n");
  PrintLocSpin(&StdI);
  PrintTrans(&StdI);
  PrintInter(&StdI);
  PrintNamelist(&StdI);
  PrintCalcMod(&StdI);
  PrintModPara(&StdI);
  Print1Green(&StdI);
  Print2Green(&StdI);
  /*
  Finalize All
  */
  free(StdI.locspinflag);
  for (ktrans = 0; ktrans < StdI.ntrans; ktrans++) {
    free(StdI.transindx[ktrans]);
  }
  free(StdI.transindx);
  free(StdI.trans);
  for (kintr = 0; kintr < StdI.nintr; kintr++) {
    free(StdI.intrindx[kintr]);
  }
  free(StdI.intrindx);
  free(StdI.intr);

  fprintf(stdout, "\n######  Input files are generated.  ######\n\n");

}
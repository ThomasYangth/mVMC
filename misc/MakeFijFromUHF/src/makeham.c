void makeham(struct BindStruct *X){

    time_t start,end;
    FILE *fp;
    char sdt[256];
    int int_i,int_j,site_1,site_2,int_spin;
    double tmp,charge;

    /* Initialize */
    for(int_spin = 0; int_spin < 2; int_spin++){
      for(int_i=0; int_i < X->Def.Nsite; int_i++){
        for(int_j=0; int_j < X->Def.Nsite; int_j++){
          X->Large.Ham[int_spin][int_i][int_j] = 0.0;
        }
      }
    }

    /*Transfer input*/
    for(int_i =0; int_i < X->Def.NTransfer; int_i++){
      site_1    = X->Def.Transfer[int_i][0];
      site_2    = X->Def.Transfer[int_i][1];
      int_spin  = X->Def.Transfer[int_i][2];
      tmp       = -X->Def.ParaTransfer[int_i];

      X->Large.Ham[int_spin][site_1][site_2] += tmp;
    }
    /*Intra U input*/
    for(int_i =0; int_i < X->Def.NCoulombIntra; int_i++){
      site_1  =  X->Def.CoulombIntra[int_i][0];
      tmp     =  X->Def.ParaCoulombIntra[int_i];
   
      X->Large.Ham[0][site_1][site_1] += tmp*X->Large.G[1][site_1][site_1];
      X->Large.Ham[1][site_1][site_1] += tmp*X->Large.G[0][site_1][site_1];
    }
    /*Inter U input*/
    for(int_i =0; int_i < X->Def.NCoulombInter; int_i++){
      site_1  =  X->Def.CoulombInter[int_i][0];
      site_2  =  X->Def.CoulombInter[int_i][1];
      tmp     =  X->Def.ParaCoulombInter[int_i];

      charge  =  X->Large.G[0][site_2][site_2]+X->Large.G[1][site_2][site_2];
      X->Large.Ham[0][site_1][site_1] += tmp*charge;
      X->Large.Ham[1][site_1][site_1] += tmp*charge;

      charge  =  X->Large.G[0][site_1][site_1]+X->Large.G[1][site_1][site_1];
      X->Large.Ham[0][site_2][site_2] += tmp*charge;
      X->Large.Ham[1][site_2][site_2] += tmp*charge;
      /*Diagonal Fock term*/
#if Fock==1
      //printf("FOOOCK\n");
      X->Large.Ham[0][site_1][site_2] += -tmp*X->Large.G[0][site_2][site_1];
      X->Large.Ham[0][site_2][site_1] += -tmp*X->Large.G[0][site_1][site_2];
      X->Large.Ham[1][site_1][site_2] += -tmp*X->Large.G[1][site_2][site_1];
      X->Large.Ham[1][site_2][site_1] += -tmp*X->Large.G[1][site_1][site_2];
#endif
    }
    /*Hund input*/
    for(int_i =0; int_i < X->Def.NHundCoupling; int_i++){
      site_1  =  X->Def.HundCoupling[int_i][0];
      site_2  =  X->Def.HundCoupling[int_i][1];
      tmp     =  -X->Def.ParaHundCoupling[int_i];
   
      X->Large.Ham[0][site_1][site_1] += tmp*X->Large.G[0][site_2][site_2];
      X->Large.Ham[1][site_1][site_1] += tmp*X->Large.G[1][site_2][site_2];

      X->Large.Ham[0][site_2][site_2] += tmp*X->Large.G[0][site_1][site_1];
      X->Large.Ham[1][site_2][site_2] += tmp*X->Large.G[1][site_1][site_1];
#if Fock==1
      X->Large.Ham[0][site_1][site_2] += -tmp*X->Large.G[0][site_2][site_1];
      X->Large.Ham[0][site_2][site_1] += -tmp*X->Large.G[0][site_1][site_2];

      X->Large.Ham[1][site_1][site_2] += -tmp*X->Large.G[1][site_2][site_1];
      X->Large.Ham[1][site_2][site_1] += -tmp*X->Large.G[1][site_1][site_2];
#endif

    }
    /*Exchange input*/
    for(int_i =0; int_i < X->Def.NExchangeCoupling; int_i++){
      site_1  =  X->Def.ExchangeCoupling[int_i][0];
      site_2  =  X->Def.ExchangeCoupling[int_i][1];
      tmp     =  X->Def.ParaExchangeCoupling[int_i];
      /*Diagonal Fock term*/
#if Fock==1
      X->Large.Ham[0][site_1][site_2] += tmp*X->Large.G[1][site_2][site_1];
      X->Large.Ham[1][site_2][site_1] += tmp*X->Large.G[0][site_1][site_2];

      X->Large.Ham[1][site_1][site_2] += tmp*X->Large.G[0][site_2][site_1];
      X->Large.Ham[0][site_2][site_1] += tmp*X->Large.G[1][site_1][site_2];
#endif
    }
    /*PariHopping input*/
    for(int_i =0; int_i < X->Def.NPairHopping; int_i++){
      site_1  =  X->Def.PairHopping[int_i][0];
      site_2  =  X->Def.PairHopping[int_i][1];
      tmp     =  X->Def.ParaPairHopping[int_i];
      /*Diagonal Fock term*/
#if Fock==1
      X->Large.Ham[0][site_1][site_2] += tmp*X->Large.G[1][site_1][site_2];
      X->Large.Ham[1][site_1][site_2] += tmp*X->Large.G[0][site_1][site_2];
#endif
    }
}
#include "makeinf.h"

char bands[NUMBANDS][40] =
{"Radio", "IR", "Optical", "UV", "X-ray", "Gamma"};

char scopes[NUMSCOPES][40] =
{"None (Artificial Data Set)", "Arecibo", "Parkes", "VLA", \
 "MMT", "Las Campanas 2.5m", "Mt. Hopkins 48in", "Other"};


void readinf(infodata * data, char *filenm)
{
  char tmp1[100], tmp2[100], infofilenm[100];
  double tmponoff[40];
  int i;
  FILE *infofile;

  sprintf(infofilenm, "%s.inf", filenm);
  infofile = chkfopen(infofilenm, "r");

  fscanf(infofile, "%*[^=]= %s", data->name);
  fscanf(infofile, "%*[^=]= %[^\n]\n", data->telescope);

  /* If not using makedata */

  if (strcmp(data->telescope, scopes[0]) != 0) {

    fscanf(infofile, "%*[^=]= %[^\n]\n", data->instrument);
    fscanf(infofile, "%*[^=]= %[^\n]\n", data->object);
    fscanf(infofile, "%*[^=]= %d:%d:%lf\n", &data->ra_h, \
	   &data->ra_m, &data->ra_s);
    fscanf(infofile, "%*[^=]= %d:%d:%lf\n", &data->dec_d, \
	   &data->dec_m, &data->dec_s);
    fscanf(infofile, "%*[^=]= %[^\n]\n", data->observer);
    fscanf(infofile, "%*[^=]= %d.%s", &data->mjd_i, \
	   tmp1);
    sprintf(tmp2, "0.%s", tmp1);
    data->mjd_f = strtod(tmp2, (char **)NULL);
/*     fscanf(infofile, "%*[^=]= %d.%lf", &data->mjd_i, \ */
/* 	   &data->mjd_f); */
/*     while (data->mjd_f > 1.0){ */
/*       data->mjd_f /= 10.0; */
/*     } */
    fscanf(infofile, "%*[^)] %*[^=]= %d", &data->bary);

  } else {

    strcpy(data->object, "fake pulsar");

  }

  fscanf(infofile, "%*[^=]= %lf", &data->N);
  fscanf(infofile, "%*[^=]= %lf", &data->dt);
  fscanf(infofile, "%*[^)] %*[^=]= %d\n", &data->numonoff);

  if (data->numonoff) {
    i = 0;
    do {
      fscanf(infofile, "%*[^=]= %lf %*[ ,] %lf", \
	     &tmponoff[i], &tmponoff[i+1]);
      i += 2;
    } while (tmponoff[i - 1] < data->N - 1 && i < 40);
    if (tmponoff[i - 1] > data->N - 1) tmponoff[i - 1] = data->N - 1;
    data->numonoff = i/2;
    data->onoff = (double *)malloc(data->numonoff*2*sizeof(double));
    for(i=0; i<data->numonoff; i++){
      data->onoff[2*i] = tmponoff[2*i];
      data->onoff[2*i+1] = tmponoff[2*i+1];
    }
  } else {
    data->numonoff = 1;
    data->onoff = (double *)malloc(2*sizeof(double));
    data->onoff[0] = 0;
    data->onoff[1] = data->N - 1;
  }

  /* If not using makedata */

  if (strcmp(data->telescope, scopes[0]) != 0) {

    fscanf(infofile, "%*[^=]= %s", data->band);

    if (strcmp(data->band, bands[0]) == 0) {

      fscanf(infofile, "%*[^=]= %lf", &data->fov);
      fscanf(infofile, "%*[^=]= %lf", &data->dm);
      fscanf(infofile, "%*[^=]= %lf", &data->freq);
      fscanf(infofile, "%*[^=]= %lf", &data->freqband);
      fscanf(infofile, "%*[^=]= %d", &data->num_chan);
      fscanf(infofile, "%*[^=]= %lf", &data->chan_wid);

    } else if ((strcmp(data->band, bands[4]) == 0) || \
	       (strcmp(data->band, bands[5]) == 0)) {

      fscanf(infofile, "%*[^=]= %lf", &data->fov);
      fscanf(infofile, "%*[^=]= %lf", &data->energy);
      fscanf(infofile, "%*[^=]= %lf", &data->energyband);

    } else {

      fscanf(infofile, "%*[^=]= %s", data->filt);
      fscanf(infofile, "%*[^=]= %lf", &data->fov);
      fscanf(infofile, "%*[^=]= %lf", &data->wavelen);
      fscanf(infofile, "%*[^=]= %lf", &data->waveband);

    }

  }
  fscanf(infofile, "%*[^=]= %[^\n]\n", data->analyzer);
  fscanf(infofile, "%[^\n]\n", tmp1);
  fscanf(infofile, "%[^\n]\n", data->notes);
  fclose(infofile);
}



void writeinf(infodata * data)
{
  char tmp1[100], tmp2[100];
  char infofilenm[100];
  int itmp, i;
  FILE *infofile;

  sprintf(infofilenm, "%s.inf", data->name);
  infofile = chkfopen(infofilenm, "w");

  fprintf(infofile, \
	  " Data file name without suffix          =  %s\n", \
	  data->name);
  fprintf(infofile, \
	  " Telescope used                         =  %s\n", \
	  data->telescope);

  if (strcmp(data->telescope, scopes[0]) != 0) {	/* If using makedata */

    fprintf(infofile, \
	    " Instrument used                        =  %s\n", \
	    data->instrument);
    fprintf(infofile, \
	    " Object being observed                  =  %s\n", \
	    data->object);
    if (data->ra_s >= 10.0) {
      fprintf(infofile, \
	      " J2000 Right Ascension (hh:mm:ss.ssss)  ");
      fprintf(infofile, "=  %.2d:%.2d:%.4f\n", \
	      data->ra_h, data->ra_m, data->ra_s);
    } else {
      fprintf(infofile, \
	      " J2000 Right Ascension (hh:mm:ss.ssss)  ");
      fprintf(infofile, "=  %.2d:%.2d:0%.4f\n", \
	      data->ra_h, data->ra_m, data->ra_s);
    }
    if (data->dec_s >= 10.0) {
      fprintf(infofile, \
	      " J2000 Declination     (dd:mm:ss.ssss)  ");
      fprintf(infofile, "=  %.2d:%.2d:%.4f\n", \
	      data->dec_d, data->dec_m, data->dec_s);
    } else {
      fprintf(infofile, \
	      " J2000 Declination     (dd:mm:ss.ssss)  ");
      fprintf(infofile, "=  %.2d:%.2d:0%.4f\n", \
	      data->dec_d, data->dec_m, data->dec_s);
    }
    fprintf(infofile, \
	    " Data observed by                       =  %s\n", \
	    data->observer);
    sprintf(tmp1, "%.15f", data->mjd_f);
    sscanf(tmp1, "%d.%s", &itmp, tmp2);
    fprintf(infofile, \
	    " Epoch of observation (MJD)             =  %d.%s\n", \
	    data->mjd_i, tmp2);
    fprintf(infofile, \
	    " Barycentered?           (1=yes, 0=no)  =  %d\n", \
	    data->bary);

  }
  fprintf(infofile, \
	  " Number of bins in the time series      =  %-11.0f\n", \
	  data->N);
  fprintf(infofile, \
	  " Width of each time series bin (sec)    =  %.15g\n", \
	  data->dt);
  fprintf(infofile, \
	  " Any breaks in the data? (1=yes, 0=no)  =  %d\n", \
	  data->numonoff > 1 ? 1 : 0);

  if (data->numonoff > 1) {
    for(i=0; i<data->numonoff; i++){
      fprintf(infofile, \
       " On/Off bin pair #%3d                   =  %-11.0f, %-11.0f\n", \
	      i + 1, data->onoff[2 * i], data->onoff[2 * i + 1]);
    }
  }

  if (strcmp(data->telescope, scopes[0]) != 0) {	/* If using makedata */

    fprintf(infofile, \
	    " Type of observation (EM band)          =  %s\n", \
	    data->band);

    if (strcmp(data->band, bands[0]) == 0) {

      fprintf(infofile, \
	      " Beam diameter (arcsec)                 =  %.0f\n", \
	      data->fov);
      fprintf(infofile, \
	      " Dispersion measure (cm-3 pc)           =  %.12g\n", \
	      data->dm);
      fprintf(infofile, \
	      " Central freq of low channel (Mhz)      =  %.12g\n", \
	      data->freq);
      fprintf(infofile, \
	      " Total bandwidth (Mhz)                  =  %.12g\n", \
	      data->freqband);
      fprintf(infofile, \
	      " Number of channels                     =  %d\n", \
	      data->num_chan);
      fprintf(infofile, \
	      " Channel bandwidth (Mhz)                =  %.12g\n", \
	      data->chan_wid);

    } else if ((strcmp(data->band, bands[4]) == 0) || \
	       (strcmp(data->band, bands[5]) == 0)) {

      fprintf(infofile, \
	      " Field-of-view diameter (arcsec)        =  %.2f\n", \
	      data->fov);
      fprintf(infofile, \
	      " Central energy (kev)                   =  %.1f\n", \
	      data->energy);
      fprintf(infofile, \
	      " Energy bandpass (kev)                  =  %.1f\n", \
	      data->energyband);

    } else {

      fprintf(infofile, \
	      " Photometric filter used                =  %s\n", \
	      data->filt);
      fprintf(infofile, \
	      " Field-of-view diameter (arcsec)        =  %.2f\n", \
	      data->fov);
      fprintf(infofile, \
	      " Central wavelength (nm)                =  %.1f\n", \
	      data->wavelen);
      fprintf(infofile, \
	      " Bandpass (nm)                          =  %.1f\n", \
	      data->waveband);
    }

  }
  fprintf(infofile, \
	  " Data analyzed by                       =  %s\n", \
	  data->analyzer);

  fprintf(infofile, \
	  " Any additional notes:\n    %s\n\n", \
	  data->notes);

  fclose(infofile);

}

#ifndef __TERMRATE_H__
#define __TERMRATE_H__

#include "iwordrank.h"
#include "sim_include.h"
#include "sim_struct.h"

int wordrank_init(const char *path);

int wordrank_release();

int feature_weight(vector<funit>& features,enum wtype type);

#endif

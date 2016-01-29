#include "termrate.h"

//wordrank dict
rank_dict_t* wordrank_dict = NULL;

/* init wordrank*/
int wordrank_init(const char * path)
{
	if(!path){
        //UB_LOG_WARNING("wordrank dict path is null");
        return -1;
    }
    wordrank_dict = wdr_create(path);
	if(wordrank_dict == NULL)
	{
		//UB_LOG_WARNING("Sorry, Failed To Load Wordrank Dict.");
		return -1;
	}
	return 0;
}


/*resource clear*/
int wordrank_release()
{
    if(wordrank_dict){
        wdr_destroy(wordrank_dict);
        wordrank_dict = NULL;
    }
    return 0;
}


// wordrank term weight calc
static int feature_weight_by_wordrank(vector<funit>& features)
{
	int i = 0;
	int count =  features.size();
	if(count == 0)
	{
		//UB_LOG_WARNING("vector size of features is 0, please check.");
		return -1;
	}

	const char* term[count];
	for(i = 0;i < count;i ++) 
	{
		term[i] = features[i].feature.c_str();
	}
	// wordrank as weight
	float rank[count];
	int ct;//the count of really processed terms
	ct = wdr_evaluate(wordrank_dict,term,count,rank);
	if(!ct || ( ct > 0 && ct < count))
	{
        for(i = 0; i < count; i ++) 
            features.at(i).weight = 1;
		//UB_LOG_WARNING("term evaluating,term weight use default value -1");
		return -1;
	}

	if(ct == count)
	{
		for(i = 0; i < count; i ++)
		{
			features.at(i).weight = rank[i];
		}
	}

	return 0;
}

int comp(funit tmp1,funit tmp2)
{
	if(tmp1.feature > tmp2.feature)
		return -1;
	else
		return 0;
}

/*获取汉字个数*/
static float _get_text_weight(const char * txt,int frequency)
{
    if(!txt){
        //UB_LOG_WARNING("parameter error");
        return -1;
    }
    int count = 0;
    float weight = 0;
    const char * cur = txt;
    int len = 0;
    
    while(*cur !='\0' && (len = ul_ccode_next_utf8(cur)) != 0)
    {
        if(ul_ccode_is_single_cnchar(cur,CODETYPE_UTF8) == 1){
            count ++;
        }
        cur += len;
    }
    if(count <=0)
        weight = frequency - frequency * 0.5;
    else if(count == 1)
        weight = frequency;
    else
        weight = frequency + frequency * ((count - 1) * 0.5);

    //UB_LOG_DEBUG("%s has %d chinese,weiht[%f]",txt,count,weight);
    return weight;
}

static int feature_weight_by_frequency(vector<funit>& features)
{
	if(features.size() == 0)
	{
		//UB_LOG_WARNING("vector size of features is 0, please check.");
		return -1;
	}
	/*sort*/
	sort(features.begin(),features.end(),comp);
    
	u_int start = 0;
	int frequency = 1;
	u_int i = 0;
    vector<funit>ftmp;
    funit fu;
    ftmp.clear();
    float weight = 0;
    
    for(i = start + 1;i < features.size();i ++)
	{
		if(features.at(i).feature != features.at(start).feature){
            fu.feature.clear();
            fu.feature = features.at(start).feature;
            weight = _get_text_weight(fu.feature.c_str(),frequency);
            fu.weight = sqrt(weight);
            ftmp.push_back(fu);
            start = i;
            frequency = 1;
		}
		else
			frequency ++;
	}

	if(start < i)
    {
        fu.feature.clear();
        fu.feature = features.at(start).feature;
        weight = _get_text_weight(fu.feature.c_str(),frequency);
        fu.weight = sqrt(weight);
        ftmp.push_back(fu);

    }
    swap(features,ftmp);
	return 0;
}


/*
 *type:which strategy to calculate feature's weight
 *type = 0 calculate feature's weight by frequency the feature appears in doc 
 */ 
int feature_weight(vector<funit>& features,enum wtype type)
{
    switch(type)
    {
        case DEGREE:
            feature_weight_by_frequency(features);
            break;
        case WORDRANK:
            feature_weight_by_wordrank(features);
            break;
        default:
            //UB_LOG_WARNING("weight calc method error");
    }

	return 0;
}

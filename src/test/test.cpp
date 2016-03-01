#include "dialogue.h"
#include "Da_Search.h"
#include "Doc_Search.h"
#include "EditDist.h"
#include "gary_string.h"

using namespace std;

const string SINGER_DA_INDEX_PATH = "../../data/index_dir/singer.txt.da.Index";
const string  SONG_DA_INDEX_PATH = "../../data/index_dir/song.txt.da.Index";
const string  ALBUM_DA_INDEX_PATH = "../../data/index_dir/album.txt.da.Index";
//const string  LRC_DA_INDEX_PATH = "../../data/index_dir/lrc.txt.da.Index";
const string  MUSIC_DOC_INDEX_PATH = "../../data/index_dir/singer.song.album.doc.Index";
const string  QUERY_DOC_INDEX_PATH = "../../data/index_dir/sogou.query.post.doc.Index";
const char* dir_path = "../../";

map<string, string> output;

int32_t AbstractInfo(string values, string& singer, string& song, string& album)
{
	int start;
	int loc_song;
	int loc_album;
	int loc_singer;
	int loc_lrc;
	
	loc_song = values.find("song]:");
	loc_album = values.find("[album]:");
	loc_singer = values.find("[singer]:");
	loc_lrc = values.find("[lrc]:");

	if(loc_song != -1 && loc_album > loc_song + 6)
	{
		song = values.substr(loc_song + 6, loc_album - loc_song - 6);
	}
	if(loc_album != -1 && loc_singer > loc_album + 8)
	{
		album = values.substr(loc_album + 8, loc_singer - loc_album - 8);
	}
	if(loc_singer != -1 && loc_lrc > loc_singer + 9)
	{
		singer = values.substr(loc_singer + 9, loc_lrc - loc_singer - 9);
	}

	// filter
	if(singer.length() > 20)
	{
		cerr << "[NOTE] singer's length is too long, probabily wrong content." << endl;
		return -1;
	}
	return 0;
}

int show_results(map<string, string>& da_vec, string type)
{
	map<string, string>::iterator iter;
	vector<string> values;
	string singer, song, album;
	for(iter = da_vec.begin(); iter != da_vec.end(); iter++)
	{
		spaceGary::StringSplit(iter->second, values, "|||");
		for(int i = 0; i < values.size(); i++)
		{
			singer = "";
			song = "";
			album = "";
			if(AbstractInfo(values[i], singer, song, album) == -1)
				continue;
			if(type == "singer")
			{
				cout << "\t[song]: " << song << "\t[album]: " << album << endl;;
			}
			if(type == "song")
			{
				cout << "\t[singer]: " << singer << "\t[album]: " << album << endl;;
			}
			if(type == "album")
			{
				cout << "\t[song]: " << song << "\t[singer]: " << singer << endl;;
			}
		}
	}
}

int show_results(map<int, string> cadidate_sort, vector<int> sortED)
{
	vector<string> tmp;
	int edis;
	for(int i = 0; i < sortED.size(); i++)
	{
		edis = sortED[i];
		tmp.clear();
		spaceGary::StringSplit(cadidate_sort[edis], tmp, "###");
		cout << "Cadidate:" << sortED[i] << endl;
		for(int j = 0; j < tmp.size(); j++)
		{
			cout << "\t\t" << tmp[j] << endl;
		}
	}

	return 0;
}

int main(int arg, char** argv)
{
	DA_SEARCH da_singersearch;
	DA_SEARCH da_songsearch;
	DA_SEARCH da_albumsearch;
	//DA_SEARCH da_lrcsearch;
	da_singersearch.Init(SINGER_DA_INDEX_PATH.c_str());
	da_songsearch.Init(SONG_DA_INDEX_PATH.c_str());
	da_albumsearch.Init(ALBUM_DA_INDEX_PATH.c_str());
	//da_lrcsearch.Init(LRC_DA_INDEX_PATH.c_str());
	
	DOC_SEARCH doc_search;
	//doc_search.Init(MUSIC_DOC_INDEX_PATH.c_str());
	doc_search.Init(QUERY_DOC_INDEX_PATH.c_str());
	
	string query;
	map<string, string> da_vec;
	DlgResult doc_result;
	vector<string> tmp;
	cerr << "[INFO] All init operation is OK" << endl;
	// segment
	SEGMENT_1 * seg = SEGMENT_1::getInstance();
	seg->Init();

	EditDist ed;

	vector<int> sortED;
	map<int, string> cadidate_sort;

	cerr << "[PLEASE INPUT A QUERY]:" << endl;
	while(true)
	{
		cerr << "[INPUT]:" << endl;
		while(cin >> query)
		{
			output.clear();
			if(query == "exit")
			{
				cerr << "[THANKS FOR VISITING HUZI PARK]" << endl << "[WELCOME ANY TIME TO BACK]"<< endl << "[I'LL MISS U.]" << endl << "[C U ~]" << endl;
				doc_search.Release();
				da_singersearch.Release();
				da_songsearch.Release();
				da_albumsearch.Release();
				//da_lrcsearch.Release();
				return 0;
			}
			da_singersearch.GetIndexResults(query, da_vec, tmp, seg);
			cerr << "[INFO] singer search is OK" << endl;
			show_results(da_vec, "singer");
			da_songsearch.GetIndexResults(query, da_vec, tmp, seg);
			cerr << "[INFO] song search is OK" << endl;
			show_results(da_vec, "song");
			da_albumsearch.GetIndexResults(query, da_vec, tmp, seg);
			cerr << "[INFO] song search is OK" << endl;
			show_results(da_vec, "album");
			//da_lrcsearch.GetIndexResults(query, da_vec, tmp, seg);
			//show_results(da_vec, "lrc");
			
			cerr << "[INFO] begin to make doc search" << endl;
			doc_result.data.clear();
			doc_search.GetIndexResults(query, doc_result, seg);
			cerr << "[INFO] doc search is OK" << endl;
			tmp = doc_result.data;
			/*for(int i = 0; i < tmp.size(); i++)
			{
				if(output.find(tmp[i]) != output.end())
					continue;
				else
				{
					string out = tmp[i];
					output[out] = 1;
					cerr << "[doc_search]\t" << tmp[i] << endl;
				}
			}*/
			sortED.clear();
			cadidate_sort.clear();
			ed.EditSort(query,tmp,cadidate_sort, sortED, seg);
			
			// show results
			//show_results(cadidate_sort, sortED);

			cadidate_sort.clear();
			sortED.clear();
			cerr << "----------------------------------------------------------------------------------------------------" << endl;
		}
	}
	da_singersearch.Release();
	da_songsearch.Release();
	da_albumsearch.Release();
	//da_lrcsearch.Release();

	doc_search.Release();
	SEGMENT_1::releaseInstance();

	return 0;
}

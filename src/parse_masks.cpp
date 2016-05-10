#include "sdsl/suffix_arrays.hpp"
#include "sdsl/wavelet_trees.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace sdsl;

//need to add assertions

uint64_t parse_masks(std::vector<uint64_t>& mask_s, std::vector<int>& mask_a, string sites_fname, string alleles_fname, std::vector<std::vector<int>>& covgs) {
  int no_alleles,a;
  uint64_t d,no_sites;
  std::ifstream h1(sites_fname);
  std::ifstream h2(alleles_fname);
  std::vector<int> v; 

  no_sites=0;
  while (h1>>d)
  { 
	  if (d>no_sites) {
		  no_sites=d;
		  mask_s.push_back(0); //fix bug in mask generation script that was missing the 0 from the beginning of each site
		  covgs.push_back(v);
	  }
	  mask_s.push_back(d);
  }
  h1.close();
  //  covgs.reserve((no_sites-4)/2);
 
  no_alleles=0;
  int i=0;
  //assert at least 2 alleles at each site
  while (h2>>a)
    { 
      if (a>no_alleles)
	no_alleles=a;
      if (a<no_alleles && a!=0) {
	covgs[(mask_s[i]-7)/2].assign(no_alleles,0); //should have -5? might change anyway if I don't end up using mask_s
	no_alleles=a;
      }
      i++;
      mask_a.push_back(a);
    }
  h2.close();

  if (no_alleles>0) {
    //    covgs.back().assign(no_alleles,0);
    covgs[covgs.size()-1].assign(no_alleles,0);
  }

  for (i=3141175;i<=3413540;i++) {
    mask_s.push_back(0);
    mask_a.push_back(0);
  }

  cout<<endl<<mask_s.size()<<" "<<mask_a.size()<<endl;

  cout<<"Covgs dim"<<covgs.size()<<" "<<covgs.front().size()<<" "<<covgs.back().size()<<endl;
  return(no_sites+1); //no_sites is last odd number in mask_sites, but alphabet size is the even number corresponding to it
}

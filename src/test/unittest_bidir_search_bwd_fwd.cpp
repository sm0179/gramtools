#include "sdsl/suffix_arrays.hpp"
#include "sdsl/wavelet_trees.hpp"
#include "gtest/gtest.h"
#include "bwt_search.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace sdsl;
using namespace std;

string q,test_file2,query,mask_file;
std::vector<uint8_t> p;
std::vector<std::vector<int>> covgs;
string prg,prg2;
vector<string> substrings;
std::vector<int> mask_a;

//forward declare
vector<string> generate_all_substrings(string q);

TEST(BackwardSearchTest, NoVariants1){

  //PRG
  test_file2="../test_cases/one_byte.txt";

  //generate all substrings of PRG, use them all as queries
  string temp;
  ifstream ff(test_file2);
  ff >> temp;//in this case, is a one char PRG, no need for substrings
  substrings.push_back(temp);

  //dummy mask
  uint32_t a;
  mask_a.clear();
  for (a=0; a< temp.length(); a++)
    {
      mask_a.push_back(0);
    }



  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  //  std::vector<int> mask_a;
 
  for (vector<string>::iterator it=substrings.begin();it<substrings.end();++it) {
    q=*it;

    bool first_del=false;
    bool precalc = false;
    int occ_expt=1;

    for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
    }

    std::vector<uint8_t>::iterator res_it=
      bidir_search_bwd(csa,0,csa.size(),0,csa.size(),
		       p.begin(),p.end(), 
		       sa_intervals,sa_intervals_rev,sites,
		       mask_a,4,first_del, precalc);

    uint64_t no_occ=(*sa_intervals.begin()).second-(*sa_intervals.begin()).first;
    EXPECT_TRUE(first_del==false);
    EXPECT_EQ(1,sa_intervals.size());
    EXPECT_EQ(no_occ,occ_expt);
    EXPECT_EQ(1, sites.size());
    EXPECT_EQ(sites.front().size(),0);
    sa_intervals.clear();
    sa_intervals_rev.clear();
    sites.clear();

    /*
    csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
    first_del=false;
    res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,4,first_del, precalc);  

    no_occ=(*sa_intervals.begin()).second-(*sa_intervals.begin()).first;
    EXPECT_TRUE(first_del==false);
    EXPECT_EQ(1,sa_intervals.size());
    EXPECT_EQ(no_occ,occ_expt);
    EXPECT_EQ(1, sites.size());
    EXPECT_EQ(sites.front().size(),0);


    sa_intervals.clear();
    sa_intervals_rev.clear();
    sites.clear();
    */
    p.clear();
  }

}







TEST(BackwardSearchTest, OneSNP){


  //PRG = catttacaca5g6t5aactagagagca
  test_file2="../test_cases/one_snp.txt";
  query="ttacacagaactagagag";//aligns across SNP allele 1 (and both flanks)
  mask_file="../test_cases/one_snp_mask_a.txt";
  ifstream g(mask_file);
  bool precalc=false;

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2,"int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;

  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,6,first_del, precalc);

  uint64_t no_occ=(*sa_intervals.begin()).second-(*sa_intervals.begin()).first;
  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);
  EXPECT_EQ(sites.front().front().first, 5);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);
  EXPECT_TRUE(sites.size()==1);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;
  /*
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,6,first_del,precalc);  

  no_occ=(*sa_intervals.begin()).second-(*sa_intervals.begin()).first;
  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);
  EXPECT_EQ(sites.front().front().first, 5);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);
  */

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  p.clear();
}


TEST(BackwardSearchTest, TwoSNPs){

  //prg = catttacaca5g6t5aactag7a8g7agcagggt
  test_file2="../test_cases/two_snps.txt";
  query="ttacacagaactagaagcag";//aligns across both SNPs, both allele 1
  mask_file="../test_cases/two_snps_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),
							 p.begin(),p.end(), 
							 sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);

  uint64_t no_occ=(*sa_intervals.begin()).second-(*sa_intervals.begin()).first;
  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);
  EXPECT_EQ(sites.front().front().first, 7);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);
  EXPECT_EQ(sites.front().back().first, 5);
  EXPECT_EQ(sites.front().back().second.front(), 1);
  EXPECT_EQ(sites.front().back().second.size(), 1);
  


  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);  

  no_occ=(*sa_intervals.begin()).second-(*sa_intervals.begin()).first;
  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);

  EXPECT_EQ(sites.front().front().first, 5);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);
  EXPECT_EQ(sites.front().back().first, 7);
  EXPECT_EQ(sites.front().back().second.front(), 1);
  EXPECT_EQ(sites.front().back().second.size(), 1);//fails - get 3



  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}


TEST(BackwardSearchTest, Two_matches_one_variable_one_nonvariable_region){

  //prg=catttacaca5g6t5aactagagagcaacagaactctct
  test_file2="../test_cases/two_matches_var_nonvar.txt";
  query="acagaac";//one match crosses allele 1, and the other in nonvar
  mask_file="../test_cases/two_matches_var_nonvar_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,6,first_del, precalc);

  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;
  
  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(2,sa_intervals.size());
  EXPECT_EQ(no_occ,2);

  //first SA_interval is in non-variable region
  EXPECT_EQ(sites.front().size(), 0);
  //second overlaps a site
  EXPECT_EQ(sites.back().front().first, 5);
  EXPECT_EQ(sites.back().front().second.front(), 1);
  EXPECT_EQ(sites.back().size(), 1);

  


  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,6,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(2,sa_intervals.size());
  EXPECT_EQ(no_occ,2);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}


TEST(BackwardSearchTest, Two_matches_one_variable_second_allele_one_nonvariable_region){

  //prg=catttacaca5g6t5aactagagagcaacataactctct
  test_file2="../test_cases/two_matches_var_other_allele_nonvar.txt";
  query="acataac";//one match crosses allele 2, and the other in nonvar
  mask_file="../test_cases/two_matches_var_nonvar_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,6,first_del, precalc);

  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;
  
  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(2,sa_intervals.size());
  EXPECT_EQ(no_occ,2);

  //first SA_interval is in non-variable region
  EXPECT_EQ(sites.front().size(), 0);
  //second SA  overlaps a site
  EXPECT_EQ(sites.back().front().first, 5);
  EXPECT_EQ(sites.back().front().second.front(), 2);
  EXPECT_EQ(sites.back().size(), 1);

  


  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,6,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(2,sa_intervals.size());
  EXPECT_EQ(no_occ,2);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}


TEST(BackwardSearchTest, Two_long_sites){


  //prg = acgacacat5gatag6tagga6gctcg6gctct5gctcgatgactagatagatag7cga8cgc8tga8tgc7ggcaacatctacga
  test_file2="../test_cases/two_long_sites.txt";

  //read aligns from middle of  allele 3 of site 5 and allele 1 of site 7
  query="gctcggctcgatgactagatagatagcgaggcaac";
  mask_file="../test_cases/two_long_sites_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=
    csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);

  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it) 
    no_occ+=(*it).second-(*it).first;

  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);

  EXPECT_EQ(sites.front().front().first, 7);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);

  //note this unit test allows for an implementation limitation
  //of gramtools right now - unless a read crosses an odd number, it is not stored in sites()
  EXPECT_EQ(sites.front().back().first, 5);
  //should really say it has overlapped allele 3
  EXPECT_EQ(sites.front().back().second.size(), 0);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}


TEST(BackwardSearchTest, Match_within_long_site_match_outside){


  //prg=gacatagacacacagt5gtcgcctcgtcggctttgagt6gtcgctgctccacacagagact5ggtgctagac7c8a7tcagctgctccacacagaga
  test_file2="../test_cases/match_within_long_site.txt";

  //read aligns in allele 2 of site 5, and in non-var region
  query="ctgctccacacagaga";
  mask_file="../test_cases/match_within_long_site_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=
    csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);

  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it) 
    no_occ+=(*it).second-(*it).first;

  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,2);



  //first SA_interval is in non-variable region
  EXPECT_EQ(sites.front().size(), 0);
  //second overlaps a site, HOWEVER it does not cross the digit 5.
  //note this unit test allows for an implementation limitation
  //of gramtools right now - unless a read crosses an odd number, it is not stored in sites()
  EXPECT_EQ(sites.back().size(), 0);




  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;

  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,2);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}



TEST(BackwardSearchTest, Long_site_and_repeated_snp_on_edge_of_site){

  //prg = gacatagacacacagt5gtcgcctcgtcggctttgagt6gtcgctgctccacacagagact5ggtgctagac7c8a7ccagctgctccacacagaga
  test_file2="../test_cases/repeated_snp_on_both_edges.txt";
  //read aligns across sites 5 and 7, allele 1 in both cases
  query="tagacacacagtgtcgcctcgtcggctttgagtggtgctagacccca";
  mask_file="../test_cases/match_within_long_site_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);


  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true,false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  bool first_del=false;
  bool precalc=false;

  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);
  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it) 
    no_occ+=(*it).second-(*it).first;

  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);

  EXPECT_EQ(sites.front().front().first, 7);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);
  EXPECT_EQ(sites.front().back().first, 5);
  EXPECT_EQ(sites.front().back().second.front(), 1);
  EXPECT_EQ(sites.front().back().second.size(), 1);


  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false,false);
  first_del=false;
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);

  EXPECT_EQ(sites.front().front().first, 5);
  EXPECT_EQ(sites.front().front().second.front(), 1);
  EXPECT_EQ(sites.front().front().second.size(), 1);//currently fails in unit test
  EXPECT_EQ(sites.front().back().first, 7);
  EXPECT_EQ(sites.front().back().second.front(), 1);
  EXPECT_EQ(sites.front().back().second.size(), 1);//currently fails

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}




TEST(BackwardSearchTest, Multiple_matches_over_multiple_sites){

  //prg=acgacacat5gatag6tagga6gctcg6gctct5gctcgtgataatgactagatagatag7cga8cgc8tga8tgc7taggcaacatctacga
  test_file2="../test_cases/multiple_matches_multiple_sites.txt";
  //read aligns over allele 1 of site 5, the nonvariableregion and allele 3 of site 7
  query="tgata";
  mask_file="../test_cases/multiple_matches_multiple_sites_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);


  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;

  //each element on the list corresponds to a SA interval
  //these elements are vectors of pairs (pair=(site, list of alleles))
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>>::iterator list_it;
  
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);
  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it) 
    no_occ+=(*it).second-(*it).first;

  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(3,sa_intervals.size());
  EXPECT_EQ(no_occ,3);
  list_it = sites.begin();

  //note this unit test allows for an implementation limitation
  //of gramtools right now - unless a read crosses an odd number, it is not stored in sites()
  //should really notice the read has overlapped allele 3 of site 7, but it does not.


  //first SA interval will be the match in the nonvariable region.
  //so we should get a vector of length zero, as it crosses no sites.
  EXPECT_EQ( (*list_it).size(), 0);
  
  //move to next SA interval - next element of list (sites)
  ++list_it;
  
  //this will be the overlap with site 7
  EXPECT_EQ( (*list_it).size(), 1);
  EXPECT_EQ((*list_it).front().first, 7);
  //  EXPECT_EQ(sites.front().front().second.front(), 3);
  EXPECT_EQ((*list_it).front().second.size(), 0);

  //next SA interval - overlap with site 5
  ++list_it;
  EXPECT_EQ( (*list_it).size(), 1);
  EXPECT_EQ((*list_it).front().first, 5);
  EXPECT_EQ( (*list_it).front().second.front(), 1);
  EXPECT_EQ((*list_it).front().second.size(), 1);


  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();

  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;
  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,8,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_TRUE(first_del==false);
  EXPECT_EQ(3,sa_intervals.size());
  EXPECT_EQ(no_occ,3);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
}

TEST(BackwardSearchTest, One_match_many_sites){


  //prg=agggccta5c6t5acatgatc7a8g7tgatca9c10a9cata11g12t11aggtcgct13c14g13ggtc15atc16cat15ttcg
  test_file2="../test_cases/One_match_many_sites.txt";
  //overlaps site5-allele1, site7-allele2, site9-allele1, site11-allele1,  site13-allele2, site15-allele2
  query="cctacacatgatcgtgatcaccatagaggtcgctgggtccat";
  mask_file="../test_cases/One_match_many_sites_mask_a.txt";
  ifstream g(mask_file);

  int a;
  mask_a.clear();
  while (g>>a) mask_a.push_back(a);

  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa=csa_constr(test_file2, "int_alphabet_file","memory_log_file","csa_file",true, false);

  std::list<std::pair<uint64_t,uint64_t>> sa_intervals, sa_intervals_rev;
  std::list<std::pair<uint64_t,uint64_t>>::iterator it;
  std::list<std::vector<std::pair<uint32_t, std::vector<int>>>> sites;

  
  bool first_del=false;
  bool precalc=false;
  q=query;
  for (uint16_t i=0;i<q.length();i++) {
       if (q[i]=='A' or q[i]=='a') p.push_back(1);
       if (q[i]=='C' or q[i]=='c') p.push_back(2);
       if (q[i]=='G' or q[i]=='g') p.push_back(3);
       if (q[i]=='T' or q[i]=='t') p.push_back(4);
  }

  std::vector<uint8_t>::iterator res_it=bidir_search_bwd(csa,0,csa.size(),0,csa.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,16,first_del, precalc);
  uint64_t no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it) 
    no_occ+=(*it).second-(*it).first;

  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);
  std::vector<std::pair<uint32_t, std::vector<int> > >::iterator  v_it = sites.front().begin();

  //here's what we are checking
  //overlaps site5-allele1, site7-allele2, site9-allele1, site11-allele1,  site13-allele2, site15-allele2


  EXPECT_EQ(sites.front().size(), 6);
  
  EXPECT_EQ((*v_it).first, 15);
  EXPECT_EQ((*v_it).second.front(), 2);
  EXPECT_EQ((*v_it).second.size(), 1);

  ++v_it;
  EXPECT_EQ((*v_it).first, 13);
  EXPECT_EQ( (*v_it).second.front(), 2);
  EXPECT_EQ((*v_it).second.size(), 1);

  //next SA interval - overlap with site 11
  ++v_it;
  EXPECT_EQ((*v_it).first, 11);
  EXPECT_EQ( (*v_it).second.front(), 1);
	     EXPECT_EQ((*v_it).second.size(), 1);

  //next SA interval - overlap with site 9
  ++v_it;
  EXPECT_EQ((*v_it).first, 9);
  EXPECT_EQ( (*v_it).second.front(), 1);
  EXPECT_EQ((*v_it).second.size(), 1);

  //next SA interval - overlap with site 7
  ++v_it;
  EXPECT_EQ((*v_it).first, 7);
  EXPECT_EQ( (*v_it).second.front(), 2);
  EXPECT_EQ((*v_it).second.size(), 1);
	     
	     
  //next SA interval - overlap with site 5
  ++v_it;
  EXPECT_EQ((*v_it).first, 5);
  EXPECT_EQ( (*v_it).second.front(), 1);
  EXPECT_EQ((*v_it).second.size(), 1);


  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  /*
  csa_wt<wt_int<bit_vector,rank_support_v5<>>,2,16777216> csa_rev=csa_constr(test_file2,"int_alphabet_file","memory_log_file","csa_file",false, false);
  first_del=false;

  res_it=bidir_search_fwd(csa_rev,0,csa_rev.size(),0,csa_rev.size(),p.begin(),p.end(), sa_intervals,sa_intervals_rev,sites,mask_a,16,first_del, precalc);  

  no_occ=0;
  for (it=sa_intervals.begin();it!=sa_intervals.end();++it)
    no_occ+=(*it).second-(*it).first;

  EXPECT_EQ(true,first_del);
  EXPECT_EQ(1,sa_intervals.size());
  EXPECT_EQ(no_occ,1);

  sa_intervals.clear();
  sa_intervals_rev.clear();
  sites.clear();
  */
  p.clear();
	     }



vector<string> generate_all_substrings(string q) {

  vector<string> substrings;
  int n,c,i;

  n=q.size();
  for (c=0; c<n; c++) {
    for (i=1; i<=n-c; i++) {
      substrings.push_back(q.substr(c,c+i));
    }
  }
  return(substrings);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);


  return RUN_ALL_TESTS();
}

#pragma once

#include <memory>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

namespace HistUtils
{

	// create the appropriate histogram given the dimensionality and precision of columns.
	// e.g. makeHist<1,float> -> TH1F
	template <unsigned int Dim, typename Prec>
	std::shared_ptr<TH1> makeHist(const std::vector<double>& xbins={0.0,1.0}, const std::vector<double>& ybins={0.0,1.0}, const std::vector<double>& zbins={0.0,1.0})
	{
		std::shared_ptr<TH1> hist;
		if constexpr(Dim==1) {
			(void)ybins;
			(void)zbins;
			if constexpr(std::is_same_v<Prec,char> || std::is_same_v<Prec,bool>) {
				hist = std::shared_ptr<TH1C>(new TH1C("","",xbins.size()-1,&xbins[0]));
			} else if constexpr(std::is_same_v<Prec,int>) {
				hist = std::shared_ptr<TH1I>(new TH1I("","",xbins.size()-1,&xbins[0]));
			} else if constexpr(std::is_same_v<Prec,float>) {
				hist = std::shared_ptr<TH1F>(new TH1F("","",xbins.size()-1,&xbins[0]));
			} else if constexpr(std::is_same_v<Prec,double>) {
				hist = std::shared_ptr<TH1D>(new TH1D("","",xbins.size()-1,&xbins[0]));
			}
		} else if constexpr(Dim==2) {
			(void)zbins;
			if constexpr(std::is_same_v<Prec,char> || std::is_same_v<Prec,bool>) {
				hist = std::shared_ptr<TH2C>(new TH2C("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0]));
			} else if constexpr(std::is_same_v<Prec,int>) {
				hist = std::shared_ptr<TH2I>(new TH2I("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0]));
			} else if constexpr(std::is_same_v<Prec,float>) {
				hist = std::shared_ptr<TH2F>(new TH2F("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0]));
			} else if constexpr(std::is_same_v<Prec,double>) {
				hist = std::shared_ptr<TH2D>(new TH2D("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0]));
			}
		} else if constexpr(Dim==3) {
			if constexpr(std::is_same_v<Prec,char> || std::is_same_v<Prec,bool>) {
				hist = std::shared_ptr<TH3C>(new TH3C("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0],zbins.size()-1,&zbins[0]));
			} else if constexpr(std::is_same_v<Prec,int>) {
				hist = std::shared_ptr<TH3I>(new TH3I("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0],zbins.size()-1,&zbins[0]));
			} else if constexpr(std::is_same_v<Prec,float>) {
				hist = std::shared_ptr<TH3F>(new TH3F("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0],zbins.size()-1,&zbins[0]));
			} else if constexpr(std::is_same_v<Prec,double>) {
				hist = std::shared_ptr<TH3D>(new TH3D("","",xbins.size()-1,&xbins[0],ybins.size()-1,&ybins[0],zbins.size()-1,&zbins[0]));
			}
		}
		hist->SetDirectory(nullptr);
		return hist;
	}

	// create the appropriate histogram given the dimensionality and precision of columns.
	// e.g. makeHist<1,float> -> TH1F
	template <unsigned int Dim, typename Prec>
	std::shared_ptr<TH1> makeHist(size_t nxbins, double xmin, double xmax, size_t nybins=1, double ymin=0, double ymax=1, size_t nzbins=1, double zmin=0, double zmax=1)
	{
		std::shared_ptr<TH1> hist = nullptr;
		if constexpr(Dim==1) {
			(void)nybins; (void)ymin; (void)ymax;
			(void)nzbins; (void)zmin; (void)zmax;
			if constexpr(std::is_same_v<Prec,char> || std::is_same_v<Prec,bool>) {
				hist = std::shared_ptr<TH1C>(new TH1C("","",nxbins,xmin,xmax));
			} else if constexpr(std::is_same_v<Prec,int>) {
				hist = std::shared_ptr<TH1I>(new TH1I("","",nxbins,xmin,xmax));
			} else if constexpr(std::is_same_v<Prec,float>) {
				hist = std::shared_ptr<TH1F>(new TH1F("","",nxbins,xmin,xmax));
			} else if constexpr(std::is_same_v<Prec,double>) {
				hist = std::shared_ptr<TH1D>(new TH1D("","",nxbins,xmin,xmax));
			}
		} else if constexpr(Dim==2) {
			(void)nzbins; (void)zmin; (void)zmax;
			if constexpr(std::is_same_v<Prec,char> || std::is_same_v<Prec,bool>) {
				hist = std::shared_ptr<TH2C>(new TH2C("","",nxbins,xmin,xmax,nybins,ymin,ymax));
			} else if constexpr(std::is_same_v<Prec,int>) {
				hist = std::shared_ptr<TH2I>(new TH2I("","",nxbins,xmin,xmax,nybins,ymin,ymax));
			} else if constexpr(std::is_same_v<Prec,float>) {
				hist = std::shared_ptr<TH2F>(new TH2F("","",nxbins,xmin,xmax,nybins,ymin,ymax));
			} else if constexpr(std::is_same_v<Prec,double>) {
				hist = std::shared_ptr<TH2D>(new TH2D("","",nxbins,xmin,xmax,nybins,ymin,ymax));
			}
		} else if constexpr(Dim==3) {
			if constexpr(std::is_same_v<Prec,char> || std::is_same_v<Prec,bool>) {
				hist = std::shared_ptr<TH3C>(new TH3C("","",nxbins,xmin,xmax,nybins,ymin,ymax,nzbins,zmin,zmax));
			} else if constexpr(std::is_same_v<Prec,int>) {
				hist = std::shared_ptr<TH3I>(new TH3I("","",nxbins,xmin,xmax,nybins,ymin,ymax,nzbins,zmin,zmax));
			} else if constexpr(std::is_same_v<Prec,float>) {
				hist = std::shared_ptr<TH3F>(new TH3F("","",nxbins,xmin,xmax,nybins,ymin,ymax,nzbins,zmin,zmax));
			} else if constexpr(std::is_same_v<Prec,double>) {
				hist = std::shared_ptr<TH3D>(new TH3D("","",nxbins,xmin,xmax,nybins,ymin,ymax,nzbins,zmin,zmax));
			}
		}
		hist->SetDirectory(nullptr);
		return hist;
	}

}
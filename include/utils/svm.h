/*

  SVM 
   - only for linear kernel & rbf kernel

  Chimin Oh (sapeyes@gmail.com)

  Chonnam National University , Gwangju

*/

#ifndef RECOGBOT_UTILS_SVM_H
#define RECOGBOT_UTILS_SVM_H


#include <math.h>

namespace Recogbot {

	inline double rbf_kernel(double *x, double *y, int n, float gamma)
	{
		double sq_d2v=0;

		for(int i=0; i<n; i++)
			sq_d2v += pow(x[i]-y[i],2);

		return exp(-gamma*sq_d2v);
	};

	class SVM_Classifier
	{
	public:
		SVM_Classifier();
		~SVM_Classifier();

		void load_param(char *sv_file, int feature_dim);

		double classify(double *x); 

	private:

		int _kernel_type;
		int _num_sv;
		int _dim;

		double _gamma;
		double _b;
		double *_w;
		double **_sp_vects;
	
	};

	SVM_Classifier::SVM_Classifier(): _sp_vects(0), _kernel_type(-1), _w(0) {};
	SVM_Classifier::~SVM_Classifier()
	{
		if(_sp_vects)
		{
			for(int i=0; i<_num_sv; i++)
				delete [] _sp_vects[i];
			delete _sp_vects;
		}

		if(_w)
			delete [] _w;
	};

	void
	SVM_Classifier::load_param(char *sv_file,int feature_dim)
	{
		_dim = feature_dim;

		if(_sp_vects)
		{
			for(int i=0; i<_num_sv; i++)
				delete [] _sp_vects[i];
			delete _sp_vects;
		}

		if(_w)
			delete [] _w;
		_w = new double[_dim];
		memset(_w,0,sizeof(double)*_dim);

		float d=0,s=0,c=0,r=0,b=0;
		int nfeature = 0, ntrain =0;
		char st[10];
		double talpha = 0;
		FILE *pf = fopen(sv_file,"r");
		fscanf(pf,"SVM-light Version V6.02\n");
		fscanf(pf,"%d # kernel type\n",&_kernel_type);
		fscanf(pf,"%f # kernel parameter -d \n",&d);
		fscanf(pf,"%lf # kernel parameter -g \n",&_gamma);
		fscanf(pf,"%f # kernel parameter -s \n",&s);
		fscanf(pf,"%f # kernel parameter -r \n",&r);
		fscanf(pf,"empty# kernel parameter -u \n");
		fscanf(pf,"%d # highest feature index \n",&nfeature);
		fscanf(pf,"%d # number of training documents \n",&ntrain);
		fscanf(pf,"%d # number of support vectors plus 1 \n",&_num_sv);
		fscanf(pf,"%lf # threshold b, each following line is a SV (starting with alpha*y)\n",&_b);
		_num_sv-=1;
		_b = -_b;
		_sp_vects = new double*[_num_sv];
	
		for(int i=0; i<_num_sv; i++)
		{
			_sp_vects[i] = new double[_dim+1];
			fscanf(pf,"%lf ",&_sp_vects[i][0]);  //t*alpha
			for(int j=0; j<_dim; j++)
			{
				sprintf(st,"%d:",j+1);
				fscanf(pf,st);
				fscanf(pf,"%lf ",&_sp_vects[i][j+1]);  // x[i]
			}
			fscanf(pf,"#\n");
		}
	
		if(_kernel_type == 0)
		{
			for(int i=0; i<_num_sv; i++)
			{
				talpha = _sp_vects[i][0];
				for(int j=0; j<_dim; j++)				
					_w[j] += talpha*(_sp_vects[i][j+1]);			
			}
		}
	}

	double 
	SVM_Classifier::classify(double *x)
	{
		double wxpb = 0;

		if(_kernel_type == 0 ) //linear kernel
		{
			for(int i=0; i<_dim; i++)
				wxpb+=_w[i]*x[i];
		}
		else if(_kernel_type == 2) // rbf kernel
		{
			double talpha = 0;

			for(int i=0; i<_num_sv; i++)
			{
				talpha = _sp_vects[i][0];
				wxpb += talpha*rbf_kernel(&_sp_vects[i][1],x,_dim,_gamma);			
			}
		}

		wxpb+=_b;

		return wxpb;
	}
}
#endif

int deref(int gid,int y_len,int z_len,int z,int y)
{
int zc=(gid/y_len);
int yc=gid-zc*y_len;
return (zc-z)*y_len+(yc-y);
}

__kernel void cal_E ( __global float* Ex,__global float* Ey,__global float* Ez ,__global float* Hx,__global float* Hy,__global float* Hz,__global float* Ex_last,__global float* Ey_last,__global float* Ez_last,__global float* Hx_last,__global float* Hy_last,__global float* Hz_last, __global float* epsilon_r, __global float* y_mesh,__global float* C)
{
float Cx;
float Cy;
float Cz;
int ylen;
int zlen;
float time;
float omega;
float lambda;
float dx;
float dy;
float dz;
float dt2;
float Cmx;
float Cmy;
float Cmz;
float src_start;
float src_stop;
float clight;
uint gid;

gid= get_global_id(0);//*2+get_local_id(0);

ylen=(int)C[3];
zlen=(int)C[4];
//printf("%d %d\n",ylen,zlen);
Cx=C[0];
Cy=C[1];
Cz=C[2];

time=C[5];
omega=C[6];
lambda=C[7];
dx=C[8];
dy=C[9];
dz=C[10];
dt2=C[11];
Cmx=C[12];
Cmy=C[13];
Cmz=C[14];
src_start=C[15];
src_stop=C[16];

if (gid < ylen*zlen)
{

int z=(gid/ylen);
int y=gid-z*ylen;

//printf("%d %d %d %d\n",z,y,gid,deref(gid,ylen,zlen,0,0));
//Ex[gid]=0;

printf("gid=%d x=%d y=%d\n",gid,z,y);
//
if ((z>0)&&(z<zlen-1))
{
	if ((y>0)&&(y<ylen-1))
	{
		//printf("achmed>%d\n",z*ylen+y-1);
		float Hzd=Hz[z*ylen+y-1];
		float Hxl=Hx[(z-1)*ylen+y];
		float Hxd=Hx[z*ylen+y-1];
		float Hyl=Hy[(z-1)*ylen+y];


		Ex[gid]=Ex_last[gid]+(Hz[gid]-Hzd)*epsilon_r[gid]*Cy-(Hy[gid]-Hyl)*epsilon_r[gid]*Cz;
		Ey[gid]=Ey_last[gid]+(Hx[gid]-Hxl)*epsilon_r[gid]*Cz;
		Ez[gid]=Ez_last[gid]-(Hx[gid]-Hxd)*epsilon_r[gid]*Cy;

		if (z==3)
		{

			//printf("%f %f\n",src_start,src_stop);
			if (y==3)//((y_mesh[y]>src_start)&&(y_mesh[y]<src_stop))
			{
				float start=0.0;//y_mesh[1];
				float a=0.0;
				float b=0.0;
				float c=0.0;
				float phi=0.0;
				float theta=0.0;
				float dot=0.0;

				phi=0.0;
				theta=90.0;
				float shift=-2.0;


				a=sin((float)(theta*(2.0*M_PI_F/360.0)))*cos((float)(phi*(2.0*M_PI_F/360.0)));

				b=sin((float)(theta*(2.0*M_PI_F/360.0)))*sin((float)(phi*(2.0*M_PI_F/360.0)));

				float eval;
				eval=90.0*(2.0*M_PI_F/360.0);
				printf("rod>%f %f %d\n",eval,cos(eval),(z-1)*ylen+y);

				c=cos((float)(theta*(2.0*M_PI_F/360.0)));

				dot=tan((float)(2.0*M_PI_F*(shift)/360.0))*(y_mesh[y]-start)*2.0*M_PI_F/lambda;
				float mod=1.0;
				dot=0.0;
				Ex[gid]=mod*a*sin(dot-time*omega);
				Ey[gid]=mod*b*sin(dot-time*omega);
				Ez[gid]=mod*c*sin(dot-time*omega);
				//printf("%f %f Ez=%f %f\n",Ex[gid],Ey[gid],Ez[gid],mod*c*sin(dot-time*omega));
				printf("%f\n",mod*c*sin(dot-time*omega));

				
			}
		}

		//printf("%f %f %f\n",Ex[gid],Ey[gid],Ez[gid]);

		/*if (z==1)
		{
			float Ex_last_l=Ex_last[(z-1)*ylen+y];
			float Ey_last_l=Ey_last[(z-1)*ylen+y];
			float Ez_last_l=Ez_last[(z-1)*ylen+y];
			Ex[(z-1)*ylen+y]=Ex_last[gid]+((clight*dt2-dz)/(clight*dt2+dz))*(Ex[gid]-Ex_last_l);
			Ey[(z-1)*ylen+y]=Ey_last[gid]+((clight*dt2-dz)/(clight*dt2+dz))*(Ey[gid]-Ey_last_l);
			Ez[(z-1)*ylen+y]=Ez_last[gid]+((clight*dt2-dz)/(clight*dt2+dz))*(Ez[gid]-Ez_last_l);
		}*/

		/*if (z==zlen-2)
		{
			float Ex_last_r=Ex_last[(z+1)*ylen+y];
			float Ey_last_r=Ey_last[(z+1)*ylen+y];
			float Ez_last_r=Ez_last[(z+1)*ylen+y];
			Ex[(z+1)*ylen+y]=Ex_last[gid]+((clight*dt2-dz)/(clight*dt2+dz))*(Ex[gid]-Ex_last_r);
			Ey[(z+1)*ylen+y]=Ey_last[gid]+((clight*dt2-dz)/(clight*dt2+dz))*(Ey[gid]-Ey_last_r);
			Ez[(z+1)*ylen+y]=Ez_last[gid]+((clight*dt2-dz)/(clight*dt2+dz))*(Ez[gid]-Ez_last_r);
		}*/


	}

}

/*
if ((z>=0)&&(z<(zlen-1)))
{
	if (y==1)
	{
		float Ex_last_d=Ex_last[(z)*ylen+y-1];
		float Ey_last_d=Ey_last[(z)*ylen+y-1];
		float Ez_last_d=Ez_last[(z)*ylen+y-1];
		Ex[(z)*ylen+y-1]=Ex_last[gid]+((clight*dt2-dy)/(clight*dt2+dy))*(Ex[gid]-Ex_last_d);
		Ey[(z)*ylen+y-1]=Ey_last[gid]+((clight*dt2-dy)/(clight*dt2+dy))*(Ey[gid]-Ey_last_d);
		Ez[(z)*ylen+y-1]=Ez_last[gid]+((clight*dt2-dy)/(clight*dt2+dy))*(Ez[gid]-Ez_last_d);
	}

	if (y==(ylen-2))
	{
		float Ex_last_u=Ex_last[(z)*ylen+y+1];
		float Ey_last_u=Ey_last[(z)*ylen+y+1];
		float Ez_last_u=Ez_last[(z)*ylen+y+1];
		//printf("%ld %le %le\n",dt2,dx,clight);
		
		Ex[(z)*ylen+y+1]=Ex_last[gid]+((clight*dt2-dy)/(clight*dt2+dy))*(Ex[gid]-Ex_last_u);
		Ey[(z)*ylen+y+1]=Ey_last[gid]+((clight*dt2-dy)/(clight*dt2+dy))*(Ey[gid]-Ey_last_u);
		Ez[(z)*ylen+y+1]=Ez_last[gid]+((clight*dt2-dy)/(clight*dt2+dy))*(Ez[gid]-Ez_last_u);

	}
}
*/
//Ex_last[gid]=Ex[gid];
//Ey_last[gid]=Ey[gid];
//Ez_last[gid]=Ez[gid];
}
}

__kernel void cal_H ( __global float* Ex,__global float* Ey,__global float* Ez ,__global float* Hx,__global float* Hy,__global float* Hz,__global float* Ex_last,__global float* Ey_last,__global float* Ez_last,__global float* Hx_last,__global float* Hy_last,__global float* Hz_last, __global float* epsilon_r, __global float* y,__global float* C)
{
float Cx=C[0];
float Cy=C[1];
float Cz=C[2];
int ylen=(int)C[3];
int zlen=(int)C[4];
float time=C[5];
float omega=C[6];
float lambda=C[7];
float dx=C[8];
float dy=C[9];
float dz=C[10];
float dt2=C[11];
float Cmx=C[12];
float Cmy=C[13];
float Cmz=C[14];
const float clight=(float)3e8;
//printf("%d %d\n",get_global_id(0),get_local_id(0));
const uint gid = get_global_id(0);
if (gid < ylen*zlen)
{
int i=(gid/ylen);
int j=gid-i*ylen;

if ((i>=0)&&(i<(zlen-1)))
{
	if ((j>=0)&&(j<ylen-1))
	{
		float Ezu=Ez[i*ylen+j+1];
		float Exr=Ex[(i+1)*ylen+j];
		float Exu=Ex[i*ylen+j+1];
		float Eyr=Ey[(i+1)*ylen+j];

		Hx[gid]=Hx_last[gid]-(Ezu-Ez[gid])*Cmy+(Eyr-Ey[gid])*Cmz;
		Hy[gid]=Hy_last[gid]-(Exr-Ex[gid])*Cmz;
		Hz[gid]=Hz_last[gid]+(Exu-Ex[gid])*Cmy;	
	}
}

//Hx_last[gid]=Hx[gid];
//Hy_last[gid]=Hy[gid];
//Hz_last[gid]=Hz[gid];
}
}

__kernel void update_E ( __global float* Ex,__global float* Ey,__global float* Ez ,__global float* Hx,__global float* Hy,__global float* Hz,__global float* Ex_last,__global float* Ey_last,__global float* Ez_last,__global float* Hx_last,__global float* Hy_last,__global float* Hz_last, __global float* epsilon_r, __global float* y,__global float* C)
{
float Cx=C[0];
float Cy=C[1];
float Cz=C[2];
int ylen=(int)C[3];
int zlen=(int)C[4];
float time=C[5];
float omega=C[6];
float lambda=C[7];
float dx=C[8];
float dy=C[9];
float dz=C[10];
float dt2=C[11];
float Cmx=C[12];
float Cmy=C[13];
float Cmz=C[14];
const uint gid = get_global_id(0);//*2+get_local_id(0);
if (gid==0) C[5]+=C[11];

if (gid < ylen*zlen)
{
	Ex_last[gid]=Ex[gid];
	Ey_last[gid]=Ey[gid];
	Ez_last[gid]=Ez[gid];
}

}

__kernel void update_H ( __global float* Ex,__global float* Ey,__global float* Ez ,__global float* Hx,__global float* Hy,__global float* Hz,__global float* Ex_last,__global float* Ey_last,__global float* Ez_last,__global float* Hx_last,__global float* Hy_last,__global float* Hz_last, __global float* epsilon_r, __global float* y,__global float* C)
{
float Cx=C[0];
float Cy=C[1];
float Cz=C[2];
int ylen=(int)C[3];
int zlen=(int)C[4];
float time=C[5];
float omega=C[6];
float lambda=C[7];
float dx=C[8];
float dy=C[9];
float dz=C[10];
float dt2=C[11];
float Cmx=C[12];
float Cmy=C[13];
float Cmz=C[14];
const uint gid = get_global_id(0);//*2+get_local_id(0);
if (gid==0)
{
C[5]+=C[11];
}
if (gid < ylen*zlen)
{
Hx_last[gid]=Hx[gid];
Hy_last[gid]=Hy[gid];
Hz_last[gid]=Hz[gid];
}

}

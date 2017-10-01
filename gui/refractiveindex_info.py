# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
#
#	https://www.gpvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License v2.0, as published by
#    the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import os
import re
import time
import sys
import random
import fnmatch
from cal_path import get_materials_path
from clone import clone_material
from util_zip import write_lines_to_archive
from util_zip import zip_remove_file
from inp import inp_update_token_value
from util_zip import read_lines_from_archive
import yaml

def extract_materials(read_lines,occurrence_to_extract=0):
	seg=-1
	edge=False
	lines=[]
	start0="value=\""
	for i in range(0, len(read_lines)):
		if i>0:
			if read_lines[i].count("option")>0:
				if read_lines[i-1].count("option")==0:
					edge=True
				else:
					edge=False
				if edge==True:
					seg=seg+1

				if seg==occurrence_to_extract:
					first_chop=read_lines[i].split(start0)#read_lines[i][read_lines[i].find(start0):]
					print(first_chop)
					if len(first_chop)>1:
						first_chop=first_chop[1]
						second_chop=read_lines[i].split("\"")
						if len(second_chop)>1:
							second_chop=second_chop[1]
						lines.append(second_chop)

	lines.remove("selected")
	return lines

def extract_sub_materials(driver):
	lines=[]
	element=driver.find_element_by_name("TabContainer1$TabPanel1$DataPageSpecifics")

	read_lines=element.get_attribute('innerHTML')
	read_lines=read_lines.replace("selected=\"selected\"","")
	read_lines=read_lines.split("\n")
	print(read_lines)
	for i in range(0, len(read_lines)):
		start0="value=\""
		first_chop=read_lines[i].split(start0)
		print(first_chop)
		if len(first_chop)>1:
			first_chop=first_chop[1]
			second_chop=read_lines[i].split("\"")
			if len(second_chop)>1:
				second_chop=second_chop[1]
			lines.append(second_chop)
	
	try:
		lines.remove("selected")
	except:
		print("")
	return lines

def extract_details(driver):
	lines=[]
	try:
		element=driver.find_element_by_id("TabContainer1_TabPanel1_pDetails")
	except:
		print("no details")
		return
	read_lines=element.get_attribute('innerHTML')
	print("details",read_lines)
	
	
def extract_ref(path,driver):
	html=driver.page_source.split("\n")
	for i in range(0,len(html)):
		if html[i].count("width:535px")>0:
			cleantext = BeautifulSoup(html[i], "lxml").text
			title=re.findall(r"['\"](.*?)['\"]", cleantext)
			authors=cleantext.split(", \'")
			if len(authors)>=1:
				authors=authors[0]
			else:
				authors=""
			print(title)
			if len(title)==0:
				title=""
			else:
				title=title[0]

			digits=(re.findall('\d+', cleantext ))
			if len(digits)>=3:
				year=str(digits[-1])
				p1=str(digits[-2])
				p0=str(digits[-3])
			else:
				year=""
				p1=""
				p0=""

			print("clear text",cleantext)
			print("digits",digits)
			print("title",title)
			print("authors",authors)
			inp_update_token_value("n.ref","#ref_website",driver.current_url,archive=path)
			inp_update_token_value("n.ref","#ref_unformatted",cleantext,archive=path)
			inp_update_token_value("n.ref","#ref_title",title,archive=path)
			inp_update_token_value("n.ref","#ref_year",year,archive=path)
			inp_update_token_value("n.ref","#ref_pages",p0+"-"+p1,archive=path)
			inp_update_token_value("n.ref","#ref_authors",authors,archive=path)

			inp_update_token_value("alpha.ref","#ref_website",driver.current_url,archive=path)
			inp_update_token_value("alpha.ref","#ref_unformatted",cleantext,archive=path)
			inp_update_token_value("alpha.ref","#ref_title",title,archive=path)
			inp_update_token_value("alpha.ref","#ref_year",year,archive=path)
			inp_update_token_value("alpha.ref","#ref_pages",p0+"-"+p1,archive=path)
			inp_update_token_value("alpha.ref","#ref_authors",authors,archive=path)


			return cleantext
	return None

def extract_data(path,driver):
	html=driver.page_source.split("\n")
	for i in range(0,len(html)):
		if html[i].count("OutputDataTable")>3:	#<tr><td class=\'OutputDataTable\'>
			lines_n=[]
			lines_n.append("#gpvdm")
			lines_n.append("#title Refractive index")
			lines_n.append("#type xy")
			lines_n.append("#x_mul 1e9")
			lines_n.append("#y_mul 1.000000")
			lines_n.append("#x_label Wavelength")
			lines_n.append("#data_label Refractive index")
			lines_n.append("#x_units nm")
			lines_n.append("#data_units m^{-1}")
			lines_n.append("#logscale_x 0")
			lines_n.append("#logscale_y 0")
			lines_n.append("#section_one Materials")
			lines_n.append("#section_two Refractive index")
			lines_n.append("#data")

			lines_alpha=[]
			lines_alpha.append("#gpvdm")
			lines_alpha.append("#title Absorption")
			lines_alpha.append("#type xy")
			lines_alpha.append("#x_mul 1e9")
			lines_alpha.append("#y_mul 1.000000")
			lines_alpha.append("#x_label Wavelength")
			lines_alpha.append("#data_label Absorption")
			lines_alpha.append("#x_units nm")
			lines_alpha.append("#data_units m^{-1}")
			lines_alpha.append("#logscale_x 0")
			lines_alpha.append("#logscale_y 0")
			lines_alpha.append("#section_one Materials")
			lines_alpha.append("#section_two Absorption")
			lines_alpha.append("#data")

			data=html[i]
			data=data.replace("<td class=\"OutputDataTable\">"," ")
			data = BeautifulSoup(data, "lxml").text
			data=data.replace(u"\xa0","")
			numbers=data.split()
			lines=int(len(numbers)/5)
			for i in range(0,lines):
				pos=i*5
				lam=float(numbers[pos])*1e-9
				n=float(numbers[pos+1])
				alpha=float(numbers[pos+4])*100.0
				#print(lam,n,alpha)
				lines_n.append(str(lam)+" "+str(n))
				lines_alpha.append(str(lam)+" "+str(alpha))

			#return "hello"#cleantext
			write_lines_to_archive(path,"alpha.omat",lines_alpha,mode="l",dest="file")
			write_lines_to_archive(path,"n.omat",lines_n,mode="l",dest="file")
			zip_remove_file(path,"cost.xlsx")

			zip_remove_file(path,"n_eq.inp")
			zip_remove_file(path,"alpha_eq.inp")

			zip_remove_file(path,"n_gen.omat")
			zip_remove_file(path,"alpha_gen.omat")

	return None

def refractiveindex_info_sync():
	files = []
	search_path=os.path.join(get_materials_path(),"refractiveindex.info","database")
	for root, dirnames, filenames in os.walk(search_path):
		for file_name in fnmatch.filter(filenames, '*.yml'):
			full_file_name=os.path.join(root, file_name)
			#lines=read_lines_from_archive(full_file_name,file_name,mode="l")
			found=False
			lam=[]
			n=[]
			alpha=[]
			
			settings_stream=open(full_file_name, 'r')
			new_name=full_file_name[len(search_path)+1:]
			dest=os.path.join(get_materials_path(),"refractiveindex.info",new_name)
			dest=dest.replace(" ","_")[:-4]
			print("rod>>>>>>>>",new_name,dest)
			settingsMap=yaml.safe_load(settings_stream)
			for main in settingsMap:
				if main=="DATA":
					#print(settingsMap['DATA'])
					if settingsMap['DATA'][0]['type']=="tabulated nk":
						lines=settingsMap['DATA'][0]['data'].split("\n")
						ref=settingsMap['REFERENCES']
						lines_n=[]
						lines_n.append("#gpvdm")
						lines_n.append("#title Refractive index")
						lines_n.append("#type xy")
						lines_n.append("#x_mul 1e9")
						lines_n.append("#y_mul 1.000000")
						lines_n.append("#x_label Wavelength")
						lines_n.append("#data_label Refractive index")
						lines_n.append("#x_units nm")
						lines_n.append("#data_units m^{-1}")
						lines_n.append("#logscale_x 0")
						lines_n.append("#logscale_y 0")
						lines_n.append("#section_one Materials")
						lines_n.append("#section_two Refractive index")
						lines_n.append("#data")

						lines_alpha=[]
						lines_alpha.append("#gpvdm")
						lines_alpha.append("#title Absorption")
						lines_alpha.append("#type xy")
						lines_alpha.append("#x_mul 1e9")
						lines_alpha.append("#y_mul 1.000000")
						lines_alpha.append("#x_label Wavelength")
						lines_alpha.append("#data_label Absorption")
						lines_alpha.append("#x_units nm")
						lines_alpha.append("#data_units m^{-1}")
						lines_alpha.append("#logscale_x 0")
						lines_alpha.append("#logscale_y 0")
						lines_alpha.append("#section_one Materials")
						lines_alpha.append("#section_two Absorption")
						lines_alpha.append("#data")

						for i in range(0,len(lines)):
							l=lines[i].split()
							if len(l)==3:
								lam=float(l[0])*1e-6
								n=float(l[1])
								alpha=4*3.14159*float(l[2])/lam
								lines_n.append(str(lam)+" "+str(n))
								lines_alpha.append(str(lam)+" "+str(alpha))

						src_dir=get_materials_path()
						src_file=os.path.join(src_dir,"generic","air")
						path=dest

						clone_material(path,src_file)
						
						path=dest+"/sim.gpvdm"
						write_lines_to_archive(path,"alpha.omat",lines_alpha,mode="l",dest="file")
						write_lines_to_archive(path,"n.omat",lines_n,mode="l",dest="file")
						zip_remove_file(path,"cost.xlsx")

						zip_remove_file(path,"n_eq.inp")
						zip_remove_file(path,"alpha_eq.inp")

						zip_remove_file(path,"n_gen.omat")
						zip_remove_file(path,"alpha_gen.omat")
						
						inp_update_token_value("n.ref","#ref_website","refractiveindex.info",archive=path+"")
						inp_update_token_value("n.ref","#ref_unformatted",ref,archive=path)
						inp_update_token_value("n.ref","#ref_authors","",archive=path)
						
						inp_update_token_value("alpha.ref","#ref_website","refractiveindex.info",archive=path)
						inp_update_token_value("alpha.ref","#ref_unformatted",ref,archive=path)
						inp_update_token_value("alpha.ref","#ref_authors","",archive=path)

#					print(settingsMap,"DATA!!!")
#				except yaml.YAMLError as exc:
#					print(exc)

#			for i in range(0,len(lines)):
#				if lines[i].count("data:")>0:
#					for ii in range(i+1,len(lines)):
#						a=lines[ii].strip().split()
#						print(lines)
#						lam.append(float(a[0]))
#						lam.append(float(a[1]))
#						lam.append(float(a[2]))

					#found=True
					#break
				
			if found==True:
				print("found",file_name)
			else:
				print("not found",file_name)

			files.append(file_name)

	return
	driver = webdriver.Firefox()

	#driver.get("https://www.rodmack.com/index.html&one=1")

	driver.get("https://www2.pvlighthouse.com.au/resources/photovoltaic%20materials/refractive%20index/refractive%20index.aspx")

	time.sleep(5)

	elem = driver.find_element_by_name("btnAcknowledge")
	elem.click()

	html_source=driver.page_source.split("\n")
	lines=extract_materials(html_source,occurrence_to_extract=0)

	for i in range(0, len(lines)):
		mat_name=lines[i]

		if os.path.isdir(os.path.join(get_materials_path(),"pvlighthouse.com.au",mat_name))==False:
			print("doing",mat_name)
			driver.find_element_by_xpath("//select[@name='TabContainer1$TabPanel1$DataPageMaterial']/option[text()=\""+lines[i]+"\"]").click()
			time.sleep(5)
			measurments=extract_sub_materials(driver)
			print(measurments)

			for ii in range(0, len(measurments)):
				print("doing",measurments[ii])
				driver.find_element_by_xpath("//select[@name='TabContainer1$TabPanel1$DataPageSpecifics']/option[text()=\""+measurments[ii]+"\"]").click()
				time.sleep(5)

				clean_measurment_name=measurments[ii].replace("[","")
				clean_measurment_name=clean_measurment_name.replace("]","")
				clean_measurment_name=clean_measurment_name.replace(" ","_")
				clean_measurment_name=clean_measurment_name.replace("(","")
				clean_measurment_name=clean_measurment_name.replace(")","")


				#extract_details(driver)
				html_source=driver.page_source.split("\n")


				src_dir=get_materials_path()
				pvlighthouse=os.path.join(get_materials_path(),"pvlighthouse.com.au")
				if os.path.isdir(pvlighthouse)==False:
					os.mkdir(pvlighthouse)

				src_file=os.path.join(src_dir,"generic","air")
				dest_file=os.path.join(pvlighthouse,mat_name,clean_measurment_name)

				clone_material(dest_file,src_file)
				
				extract_ref(os.path.join(src_file,"sim.gpvdm"),driver)
				extract_data(os.path.join(src_file,"sim.gpvdm"),driver)

				time.sleep(5)
		else:
			print("done already",mat_name)


	driver.close()

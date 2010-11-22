/**
 * @brief Stopping Power
 */

/*
 *    AT_DataStoppingPower.c
 *    ==============
 *
 *    Created on: 12.11.2010
 *    Creator: kongruencja
 *
 *    Copyright 2006, 2010 The libamtrack team
 *
 *    This file is part of the AmTrack program (libamtrack.sourceforge.net).
 *
 *    AmTrack is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    AmTrack is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with AmTrack (file: copying.txt).
 *    If not, see <http://www.gnu.org/licenses/>
 */

#include "AT_DataStoppingPower.h"


double AT_Bethe_wrapper(const double 	E_MeV_u,
		const long 	    particle_no,
		const long 		material_no){
	return AT_Stopping_Power_Mass_Bethe_MeV_cm2_g_single(	E_MeV_u,
															particle_no,
															material_no,
															-1.0);
};


double AT_Stopping_Power_data_interpolation(const long stopping_power_source_no,
		const double 	E_MeV_u,
		const long 	    particle_no,
		const long 		material_no){

	assert( AT_stopping_power_tabulated_source.stopping_power_source_data_group != NULL );
	assert( AT_stopping_power_tabulated_source.number_of_sources == STOPPING_POWER_SOURCE_N);
	assert( stopping_power_source_no < AT_stopping_power_tabulated_source.number_of_sources);
	assert( stopping_power_source_no >= 0 );

	AT_stopping_power_tabulated_source_group_for_all_materials_struct * group_for_all_materials =
			(AT_stopping_power_tabulated_source_group_for_all_materials_struct *)AT_stopping_power_tabulated_source.stopping_power_source_data_group[stopping_power_source_no];

	if( group_for_all_materials != NULL ){

		assert( group_for_all_materials->material_no[material_no] == material_no);
		assert( group_for_all_materials->stopping_power_source_no == stopping_power_source_no);

		AT_stopping_power_tabulated_source_for_given_material_struct * source_for_given_material =
				(AT_stopping_power_tabulated_source_for_given_material_struct *)group_for_all_materials->stopping_power_source_data[material_no];

		if( source_for_given_material != NULL ){
			const long n = source_for_given_material->number_of_data_points;
			if( source_for_given_material->E_MeV_u_and_stopping_power_total_MeV_cm2_g != NULL){

				double result = AT_get_interpolated_y_from_input_2d_table(
						source_for_given_material->E_MeV_u_and_stopping_power_total_MeV_cm2_g,
						n,
						E_MeV_u);

				return result;
			} else {
				char source_name[1000];
				AT_stopping_power_source_model_name_from_number(stopping_power_source_no,source_name);
				char material_name[1000];
				AT_material_name_from_number(material_no,material_name);
				printf("Missing data points for data source [%s] and material [%s]\n", source_name, material_name);
			}
		} else {
			char source_name[1000];
			AT_stopping_power_source_model_name_from_number(stopping_power_source_no,source_name);
			char material_name[1000];
			AT_material_name_from_number(material_no,material_name);
			printf("Missing data for data source [%s] and material [%s]\n", source_name, material_name);
		}
	} else {
		char source_name[1000];
		AT_stopping_power_source_model_name_from_number(stopping_power_source_no,source_name);
		printf("Missing data for data source [%s]\n", source_name);
		return -1;
	}
	return -1;
};


double AT_Stopping_Power_MeV_cm2_g_single( const long stopping_power_source_no, const double E_MeV_u, const long particle_no, const long material_no){
	double Stopping_Power_MeV_cm2_g = 0.0;
	if( AT_stopping_power_analytical_source.access_function[stopping_power_source_no] != NULL){
		Stopping_Power_MeV_cm2_g = AT_stopping_power_analytical_source.access_function[stopping_power_source_no](E_MeV_u, particle_no, material_no);
	} else {
		Stopping_Power_MeV_cm2_g = AT_Stopping_Power_data_interpolation(stopping_power_source_no, E_MeV_u, particle_no, material_no);
	}

	if( particle_no != PARTICLE_PROTON_NUMBER ){
		double Zeff_ion    =  AT_effective_charge_from_E_MeV_u_single(E_MeV_u, particle_no);
		double Zeff_proton =  AT_effective_charge_from_E_MeV_u_single(E_MeV_u, PARTICLE_PROTON_NUMBER);
		Stopping_Power_MeV_cm2_g *= gsl_pow_2(Zeff_ion / Zeff_proton);
	}

	return Stopping_Power_MeV_cm2_g;
};


void AT_Stopping_Power_MeV_cm2_g_multi( const long stopping_power_source_no, const long number_of_particles, const double E_MeV_u[], const long particle_no[], const long material_no, double Stopping_Power_MeV_cm2_g[]){
	long i;
	for( i = 0 ; i < number_of_particles; i++){
		Stopping_Power_MeV_cm2_g[i] = AT_Stopping_Power_MeV_cm2_g_single(stopping_power_source_no, E_MeV_u[i], particle_no[i], material_no);
	}
}


double AT_Stopping_Power_keV_um_single( const long stopping_power_source_no, const double E_MeV_u, const long particle_no, const long material_no){

	double Stopping_Power_MeV_cm2_g = AT_Stopping_Power_MeV_cm2_g_single( stopping_power_source_no, E_MeV_u, particle_no, material_no);

    double material_density_g_cm3 = AT_density_g_cm3_from_material_no(material_no);

	return Stopping_Power_MeV_cm2_g * material_density_g_cm3 * 0.1;
};


void AT_Stopping_Power_keV_um_multi( const long stopping_power_source_no, const long number_of_particles, const double E_MeV_u[], const long particle_no[], const long material_no, double Stopping_Power_keV_um[]){
	long i;
	for( i = 0 ; i < number_of_particles; i++){
		Stopping_Power_keV_um[i] = AT_Stopping_Power_keV_um_single(stopping_power_source_no, E_MeV_u[i], particle_no[i], material_no);
	}
}


void AT_stopping_power_source_model_name_from_number( const long source_no, char* source_name){

	assert( source_no >= 0);
	assert( source_no < STOPPING_POWER_SOURCE_N);
	assert(AT_stopping_power_sources.stopping_power_source_no[source_no] == source_no);

    strcpy(source_name, AT_stopping_power_sources.stopping_power_source_name[source_no]);
}


long AT_stopping_power_source_model_number_from_name( const char* source_name ){

	long  match;
	const long n_tmp = 1;

	assert( source_name != NULL);

	find_elements_char(  &source_name,
			n_tmp,
			AT_stopping_power_sources.stopping_power_source_name,
			AT_stopping_power_sources.n,
			&match);

	return match;
}


double AT_Stopping_Power_Bethe_Number(	const double 	E_MeV_u,
										const long 		particle_no,
										const long 		material_no,
										const double	E_restricted_keV)
{
	  const double beta2 	= gsl_pow_2(AT_beta_from_E_single(E_MeV_u));
	  const double I_MeV	= AT_I_eV_from_material_no(material_no) * 1e-6;
	  double Wm_MeV			= AT_max_relativistic_E_transfer_MeV_single(E_MeV_u);

	  /* Restricted stopping number requested? */
	  bool restricted = false;
	  if(	(E_restricted_keV > 0.0) && (E_restricted_keV / 1000.0 < Wm_MeV))
		  restricted = true;

	  /* First part of stopping number */
	  double SN11			=  2.0 * electron_mass_MeV_c2 * beta2 / (1.0 - beta2);
	  assert( I_MeV > 0. );
	  SN11					/= I_MeV;

	  if(	restricted){
		  Wm_MeV				= E_restricted_keV * 1e-3;
	  }
	  double SN12			= Wm_MeV / I_MeV;

	  /* Second part of stopping number */
	  double SN2			= beta2;
	  if(	restricted){
		  SN2					/= 2;
		  SN2					+= (1.0 - beta2) * Wm_MeV / (4.0 * electron_mass_MeV_c2);
	  }

	  /* Third part of stopping number (shell correction) TODO: implement */
	  /* Forth part of stopping number (density correction TODO: implement*/

	  assert( SN11 > 0. );
	  assert( SN12 > 0. );

	  return (0.5 * log(SN11 * SN12) - SN2);
}


double AT_Stopping_Power_Mass_Bethe_MeV_cm2_g_single(	const double E_MeV_u,
														const long particle_no,
														const long material_no,
														const double E_restricted_keV)
{
	  const double beta2 		= gsl_pow_2(AT_beta_from_E_single(E_MeV_u));
	  assert( beta2 > 0);
	  const double Z			= AT_average_Z_from_material_no(material_no);
	  const double A			= AT_average_A_from_material_no(material_no);
	  assert( A > 0 );
	  const double z_eff		= AT_effective_charge_from_E_MeV_u_single(E_MeV_u, particle_no);
	  const double k_MeV_cm2_g	= 0.307075;												// ICRU49, p.6, after Cohen and Taylor (1986)
	  const double SN			= AT_Stopping_Power_Bethe_Number(	E_MeV_u,
																	particle_no,
																	material_no,
																	E_restricted_keV);

	  return( k_MeV_cm2_g * Z / A * gsl_pow_2(z_eff) * SN / (beta2));
}


void AT_Stopping_Power_Mass_Bethe_MeV_cm2_g_multi(	const long n,
		const double E_MeV_u[],
		const long particle_no[],
		const long material_no,
		const double E_restricted_keV,
		double Mass_Stopping_Power_MeV_cm2_g[])
{
	long i;
	for (i = 0; i < n; i++){
		Mass_Stopping_Power_MeV_cm2_g[i]	=	AT_Stopping_Power_Mass_Bethe_MeV_cm2_g_single(	E_MeV_u[i],
																								particle_no[i],
																								material_no,
																								E_restricted_keV);
	}
}
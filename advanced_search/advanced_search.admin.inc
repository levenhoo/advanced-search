<?php

/**
* @file
* Admin page callbacks for the Advanced_search module.
*
*/


/*
*  @see advanced_search_menu
* */
function _advanced_search_form_setting_form($form,&$form_state,$content_type) {

	$form['format'] = array(
		'#type' => 'textarea',
		'#rows' => 15,
		'#default_value' =>  variable_get('advanced_search_'.$content_type),
	);
	$form['content_type'] = array(
		'#type' => 'hidden',
		'#default_value' => $content_type,
	);
	$form['submit'] = array(
		'#type'  =>  'submit',
		'#value' => 'Submit',
	);
	$fields = field_info_instances('node', $content_type );
	//dpm($fields);
	$output = "";
	$output.="<table>";
	$output.="<tr>";
	$output.="<td>title</td>";
	$output.="<td>[advance_search:title]</td>";
	$output.="</tr>";
	foreach($fields as $field ){
		$label=$field['label'];
		$type=$field['widget']['type'];
		$module=$field['widget']['module'];
		$description=$field['description'];
		$field_name=$field['field_name'];
	if( !_advanced_search_is_allow_search($type))  continue;
		$output.="<tr>";
		$output.="<td>".$label."</td>";
		$output.="<td>[advance_search:". $field_name."]</td>";
		$output.="</tr>";
	}
	$output.="<tr>";
	$output.="<td>submit</td>";
	$output.="<td>[advance_search:submit]</td>";
	$output.="</tr>";
	$output.="</table>";
	$form['list'] = array(
	'#markup' =>$output ,
	);
	return $form;
}

/*
*  @see _advanced_search_form_setting
* */
function _advanced_search_form_setting_form_submit($form,&$form_state) {
	variable_set('advanced_search_'.$form_state['values']['content_type'], $form_state['values']['format']);
	drupal_set_message(t('update success'));
}


function _advanced_search_is_allow_search($type){
	$allow_type = array( "text_textfield", "options_select", "options_buttons" );
	return in_array( $type , $allow_type );
}
<?php



function _welcome_form(){
	return "Advance search help.............";
}


/**
 * create the form
 * @see advanced_search_menu()
 */
function _advanced_search_customer_search_form($form,&$form_state,$content_type, $cache_key="") {
	$reset_cache = true;
	if(!($cached = cache_get($cache_key)) || $reset_cache) {
		if($cached){
			$form_state["values"] = $cached->data["form_state_values"];
		}
	}
	//standard field
	$form['title'] = array(
			'#type' => 'textfield',
			'#title' => t('title'),
			'#required' => false,
			'#default_value' => "",
			'#description' => "Please enter your search value.",
			'#size' => 30,
			'#maxlength' => 20,
	);
	if($cached){
		$form['title'][ '#default_value'] = $form_state["values"]['title'];
	}
	$form['content_type'] = array(
			'#type' => 'hidden',
			'#value' => $content_type ,
	);
	$fields = field_info_instances('node', $content_type );
	//dpm($fields);
	foreach($fields as $field ){
		$label=$field['label'];
		$type=$field['widget']['type'];
		$module=$field['widget']['module'];
		$description=$field['description'];
		$field_name=$field['field_name'];
		switch($type){
			case"text_textfield":
				$form[$field_name] = array(
						'#type' => 'textfield',
						'#title' => t( $label ),
						'#description' => $description ,
						'#size' => 30,
				);
				// if($cached)$form[$field_name]['#default_value']=$form_state['values'][$field_name];
				break;
			case "options_buttons":
				//get field allowed values
				$data = field_info_field( $field_name);
				$allowed_values = $data['settings']['allowed_values'];
				$cardinality = $data['cardinality'];//check radio or checkbox
				//dpm($allowed_values);
				//dpm($data);
				if($cardinality === '1'){ //radios
					$form[$field_name] = array(
							'#type' => 'radios',
							'#title' => t( $label ),
							'#options' => $allowed_values,
							'#description' => t( $description ),
					);
					// if($cached){$form[$field_name]['#default_value']=$form_state['values'][$field_name];}
				}else{ //checkboxes
					$form[$field_name] = array(
							'#type' => 'checkboxes',
							'#options' => drupal_map_assoc(  $allowed_values  ),
							'#title' => t( $label ),
					);
					// if($cached){$form[$field_name]['#default_value']=$form_state['values'][$field_name];}
				}
				break;
			case "options_select" :
				$data = field_info_field( $field_name);
				$field_module = $data['module'];
				if($field_module=='taxonomy'){
					$allowed_values = array();
				}else
					$allowed_values = $data['settings']['allowed_values'];
				array_unshift($allowed_values,'');
				//dpm($allowed_values);
				$form[$field_name] = array(
						'#type' => 'select',
						'#title' => t( $label ),
						'#options' => $allowed_values ,
						'#default_value' => '',
						'#description' => t($description),
				);
				// if($cached){$form[$field_name]['#default_value']=$form_state['values'][$field_name];}
				break;
			default :
				//code......
				break;
		}
		//last set default values
		if($cached){  //fetch cache submit values
			if( isset($form_state['values'][$field_name])  )
				$form[$field_name]['#default_value']= $form_state['values'][$field_name] ;
		}//end foreach
	}
	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Submit',
	);
	$form['list'] = array(
			'#markup' => _search_list($form,$form_state,$content_type,$cache_key),
	);
	return $form;
}


/*
* @see _advanced_search_form
* */
function _advanced_search_customer_search_form_submit($form,&$form_state) {
	$form_state['rebuild'] = false;
	//dpm($form_state);
	$content_type = $form_state["values"]["content_type"];
	$values = $form_state['values'];
	unset($values['form_build_id'], $values['form_token'], $values['form_id']);
	$cache_key = 'results-'.md5(serialize($values));
	$cache_data = array(
			'form_state_values' => $form_state['values']
	);
	cache_set($cache_key, $cache_data);
	//drupal_goto("advanced_search/".$content_type."/".$cache_key);
	$form_state['redirect']="advanced_search/".$content_type."/".$cache_key ;
}

/**
 *
 * query a content type node list
 * @see _advanced_search_form()
 */
function _search_list($form,$form_state,$content_type,$cache_key='') {

	$reset_cache = true;
	if(!($cached = cache_get($cache_key)) || $reset_cache) {
		if($cached){
			$form_state["values"] = $cached->data["form_state_values"];
		}
	}
	$nodeQuery = new EntityFieldQuery();
	$entities = $nodeQuery->entityCondition('entity_type', 'node')
			->entityCondition('bundle',$content_type); // to target content_type
	if(isset($form_state['values'])){
		if($form_state['values']['title'])
			$entities = $nodeQuery->propertyCondition('title', '%'.$form_state['values']['title'].'%','like');
		foreach($form as $form_key => $form_value){
			$formType =$form[$form_key]['#type'];
			$formValue =$form_state['values'][$form_key];
			if ( !_advanced_search_is_allow_key( $form_key  ) )
				if ( !_advanced_search_is_allow_type($formType) ) {
					if( empty($formValue) ) continue;
					switch($formType){

						case 'checkboxes' :
							//checkboxes
							foreach( $formValue as $checkboxValue   ){
								if($checkboxValue)//is not 0
									$entities = $nodeQuery->fieldCondition($form_key, 'value', $checkboxValue);
							}
							break;
						case 'select' :		//select
							$entities = $nodeQuery->fieldCondition($form_key, 'value', $formValue);
							break;
						case 'textfield' :		//textfield
							$entities = $nodeQuery->fieldCondition($form_key, 'value', '%'.$formValue.'%','like');
							break;
						case 'radios' :         //radios
							$entities = $nodeQuery->fieldCondition($form_key, 'value', $formValue);
							break;
						default :
							dpm('Testing:'.$formType);
							break;
					}
				}
		}
	}
	$entities = $nodeQuery-> pager();
	$entities = $nodeQuery-> execute();
	$rows = array();
	$nids = array();
	if($entities)
		foreach($entities['node'] as $entity){
			$node = node_load($entity->nid);
			$nids[] = $node->nid;
			$rows[] = array(
					$node->nid,
					$node->title
			);
		}
	$nodes = node_load_multiple($nids);//讀取NID數組獲得NODE數組
	$nodes_view = node_view_multiple($nodes);//NODE数组组成NODE_VIEW
	$output = render($nodes_view);
	$output .= theme('pager');
	//dpm($output);
	return $output;
}



/* 函数命名都应该带前缀 */

function _advanced_search_is_allow_type($type){
	$pass_type = array( "submit", "hidden" );
	return in_array( $type , $pass_type );
}
function _advanced_search_is_allow_key($key){
	$allow_key = array( "title", "submit", "form_build_id", "form_token","form_id" , "op");
	return in_array( $key , $allow_key );
}


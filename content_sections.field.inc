<?php
	
	/**
	 * Implements hook_field_info().
	 */
	function content_sections_field_info() {
	  return array(
		'content_sections' => array(
		  'label' => t('Content sections'),
		  'description' => t('Static content divided by sections.'),
		  'settings' => array(
		  ),
		  'default_widget' => 'content_sections',
		  'default_formatter' => 'content_sections',
		),
	  );
	  
	}
	
	function content_sections_field_menu(){
		return array(
			'content-sections/default-sources' => array(
				'page callback' => 'content_sections_default_source_autocomplete',
				'access arguments' => 'administer nodes',
				'type' => MENU_CALLBACK,
			)
		);
	}
		
	/**
	 * Implements hook_field_is_empty().
	 */
	function content_sections_field_is_empty($item, $field) {
		return FALSE;
	}

	/**
	 * Implements hook_field_widget_form().
	 */
	function content_sections_field_settings_form($field, $instance, $has_data) {
		$form = array();
		$settings = $instance['settings'];
		
		return $form;
	}

	/**
	 * Implements hook_field_instance_settings_form().
	 */
	function content_sections_field_instance_settings_form($field, $instance) {
		$form = array();
		
		$form['default_source'] = array(
			'#type' => 'textfield',
			'#title' => t('Default sections Provider'),
			'#autocomplete_path' => 'content-sections/default-sources',
		); 
		
		return $form;
	}

	function content_sections_default_source_autocomplete($str){
		$matches = array(); 
	
		$query = new EntityFieldQuery();
		$query->entityCondition('entity_type', 'node')
		  ->propertyCondition('title', $str, 'CONTAINS')
		  ->range(0, 20);
		  
		$result = $query->execute();
		$nodes = entity_load('node', array_keys($result['node']));
		
		//kpr($nodes);
		
		foreach ($nodes as $node) {
			$matches[$node->nid] = check_plain($node->title);
		} 
		
		drupal_json_output($matches); 

		exit();
	}

	/**
	 * Implements hook_field_presave().
	 */
	function content_sections_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
		$sort = false;
		foreach ($items as $delta => &$item) {
			if(empty($item['options'])){
				$item['options'] = null;
			}
			if(!isset($item['parent_delta']) || strlen($item['parent_delta']) == 0){
				$item['parent_delta'] = null;
			}
			if(isset($item['weight'])){
				$sort = true;
			}
		}
		$items = content_sections_update_tree_data($items,$sort,true);
		//dpm($items);
	}
	


	/**
	 * Implements hook_field_load().
	 */
	function content_sections_field_load($entity_type, $entities, $field, $instances, $langcode, &$items) {

		foreach ($entities as $id => $entity) {
			foreach ($items[$id] as $delta => $item) {
			
			}
		}
	  
	}
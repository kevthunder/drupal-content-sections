<?php
	$plugin = array(
		'title' => t('Text box'),
		'description' => t('Text box.'),
		'load callback' => 'section_text_box_load',
		'form callback' => 'section_text_box_form',
		'validate callback' => 'section_text_box_form_validate',
		'data callback' => 'section_text_box_submit',
		'save callback' => 'section_text_box_save',
		'access callback' => 'section_text_box_access',
		'view callback' => 'section_text_box_view',
    'node_type' => 'section',
	);
	
	
	function _section_text_box_get_behavior(){
		static $behavior;
		if(empty($behavior)){
			module_load_include('inc', 'content_sections', 'node_content_section');
			$behavior = new NodeContentSection('section');
		}
		return $behavior;
	}
	/**
	 * load callback
	 */
	function section_text_box_load($section,$language=null){
		$B = _section_text_box_get_behavior();
		$node = $B->load($section,$language);
		$section['label'] = $node->title;
		return $section;
	}
	/**
	 * form callback
	 */
	function section_text_box_form($container,$section,$form,&$form_state){
		$B = _section_text_box_get_behavior();
		return $B->form($container,$section,$form,$form_state);
	}
	
	/**
	 * validate callback
	 */
	function section_text_box_form_validate($form,&$form_state){
		$B = _section_text_box_get_behavior();
		return $B->validate($form,$form_state);
	}
	
	/**
	 * data callback
	 */
	function section_text_box_submit($formValues,$section,$container,$form,&$form_state){
		$B = _section_text_box_get_behavior();
		return $B->submit($formValues,$section,$container,$form,$form_state);
	}
	
	/**
	 * save callback
	 */
	function section_text_box_save($section,$entity_type,$entity,$instance,$delta,$form,&$form_state){
		$B = _section_text_box_get_behavior();
		return $B->save($section,$entity_type,$entity,$instance,$delta,$form,$form_state);
	}
	
	/**
	 * access callback
	 */
	
	function section_text_box_access($section, $op, $account){
		if($op == 'view'){
			$B = _section_text_box_get_behavior();
			$node = $B->load($section);
			if(!empty($node)){
				$section['node'] = $node;
				$classes = field_get_items('node',$node,'field_class');
				if(!empty($classes)){
					$classes = reset($classes);
					
					module_load_include('inc', 'content_sections', 'class_based_access_validator');
					$Validator = new ClassBasedAccessValidator();
					
					return $Validator->check($classes['value'],$account) ? null : false;
					
				}
			}
		}
	}
	
	/**
	 * view callback
	 */
	function section_text_box_view($row, $langcode, $display){
		$B = _section_text_box_get_behavior();
		$node = $B->load($row['#section'],$langcode);
		if(!empty($node)){
			$row['node'] = node_view($B->load($row['#section'],$langcode),'teaser',$langcode);
			$row['node']['#section'] = $row['#section'];
			$row['node']['#depth'] = $row['#section']['depth'];
			$row['node']['children'] = $row['children'];
			unset($row['children']);
			return $row;
		}
	}
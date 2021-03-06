<?php
	$plugin = array(
		'title' => t('Video box'),
		'description' => t('Video box.'),
		'form callback' => 'section_video_box_form',
		'validate callback' => 'section_video_box_form_validate',
		'data callback' => 'section_video_box_submit',
		'save callback' => 'section_video_box_save',
		'view callback' => 'section_video_box_view',
    'node_type' => 'video_box',
	);
	
	
	function _section_video_box_get_behavior(){
		static $behavior;
		if(empty($behavior)){
			module_load_include('inc', 'content_sections', 'node_content_section');
			$behavior = new NodeContentSection('video_box');
		}
		return $behavior;
	}
	
	function section_video_box_form($container,$section,$form,&$form_state){
		$B = _section_video_box_get_behavior();
		return $B->form($container,$section,$form,$form_state);
	}
	
	function section_video_box_form_validate($form,&$form_state){
		$B = _section_video_box_get_behavior();
		return $B->validate($form,$form_state);
	}
	
	function section_video_box_submit($formValues,$section,$container,$form,&$form_state){
		$B = _section_video_box_get_behavior();
		return $B->submit($formValues,$section,$container,$form,$form_state);
	}
	
	function section_video_box_save($section,$entity_type,$entity,$instance,$delta,$form,&$form_state){
		$B = _section_video_box_get_behavior();
		return $B->save($section,$entity_type,$entity,$instance,$delta,$form,$form_state);
	}
	
	function section_video_box_view($row, $langcode, $display){
		$B = _section_video_box_get_behavior();
		$node = $B->load($row['#section'],$langcode);
		if(!empty($node)){
			$row['node'] = node_view($node,'teaser',$langcode);
			$row['node']['#section'] = $row['#section'];
			$row['node']['#depth'] = $row['#section']['depth'];
			$row['node']['children'] = $row['children'];
			unset($row['children']);
			return $row;
		}
	}
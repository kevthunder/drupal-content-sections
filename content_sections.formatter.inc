<?php
	function content_sections_formatter_theme(){
		return array(
			'content_sections' => array(
				'render element' => 'form',
			),
			'content_section' => array(
				'render element' => 'form',
			),
		);
	}
	
	function content_sections_field_formatter_info() {
	  return array(
		'content_sections' => array(
		  'label' => t('Content sections'),
		  'description' => t('Static content divided by sections.'),
		  'field types' => array('content_sections'),
		  'settings' => array(
		  ),
		),
	  );
	}

	function content_sections_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
		
		$elem = array();

		return $elem;
	}

	function content_sections_field_formatter_settings_summary($field, $instance, $view_mode) {
		
		$elem = array();

		return $elem;
	}
	
	function content_sections_field_formatter_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items, $displays) {
		
	}

	function content_sections_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
		if(empty($items) && !empty($instance['settings']['default_source']) && $defProvider = node_load($instance['settings']['default_source']) ){
			//kpr($instance);
			$items = field_get_items('node', $defProvider, $instance['field_name']);
		}
		$sectionsByParent = array();
		foreach($items as $delta => $section){
			$access = true;
			$p = $section['parent_delta'];
			if(!isset($p)) $p = 'root';
			ctools_include('plugins');
			if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'access callback')){
				$res = $funct($section, 'view', $GLOBALS['user']);
				if($res === false) $access = false;
			}
			if($access){
				$sectionsByParent[$p][$delta]  = $section;
			}
		}
		$elem = _content_sections_field_formatter_view('root',$sectionsByParent, $entity_type, $entity, $field, $instance, $langcode, $display);
		return $elem;
	}
	function _content_sections_field_formatter_view($parent, $sectionsByParent, $entity_type, $entity, $field, $instance, $langcode, $display) {
		$elem = array();
		if(!empty($sectionsByParent[$parent])){
			foreach($sectionsByParent[$parent] as $delta => $section){
				$row = array(
					'#theme' => 'content_section',
					'#section' => $section,
					'children' => _content_sections_field_formatter_view($delta,$sectionsByParent, $entity_type, $entity, $field, $instance, $langcode, $display),
				);
				if(!empty($row['children'])){
					$row['children']['#theme'] = 'content_subsections';
				}
				ctools_include('plugins');
				if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'view callback')){
					$res = $funct($row, $entity->language, $display);
					if(is_array($res)){
						$row = $res;
					}elseif($res===false){
						$row['#hidden'] = true;
					}
				}
				if(empty($row['#hidden'])){
					$elem[] = $row;
				}
			}
		}
		return $elem;
	}
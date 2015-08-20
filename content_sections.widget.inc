<?php
	
	
	/**
	 * part of hook_theme() implementation, redirected by content_sections_theme().
	 */
	function content_sections_widget_theme(){
		return array(
			'content_sections_editor' => array(
				'render element' => 'form',
			),
		);
	}
	
	function content_sections_field_widget_info() {
		return array(
			'content_sections' => array(
				'label' => t('Content sections'),
				'field types' => array('content_sections'),
				'settings' => array(
					'container_collapsible' => TRUE,
					'container_collapsed' => false,
				),
				'behaviors' => array(
				  'multiple values' => FIELD_BEHAVIOR_CUSTOM,
				  'default value' => FIELD_BEHAVIOR_NONE,
				),
			),
		);
	}

	function content_sections_field_widget_settings_form($field, $instance) {
		$widget = $instance['widget'];
		$settings = $widget['settings'];
		
		$form = array();
		
		$form['container_collapsible'] = array(
			'#type' => 'checkbox',
			'#title' => t('Collapsible Container'),
			'#default_value' => isset($settings['container_collapsible'])?$settings['container_collapsible']:true,
			'#attributes' => array(
				'class' => array('label-field')
			),
			'#weight' => 5,
		);
		$form['container_collapsed'] = array(
			'#type' => 'checkbox',
			'#title' => t('Collapsed Container'),
			'#default_value' => isset($settings['container_collapsed'])?$settings['container_collapsed']:false,
			'#attributes' => array(
				'class' => array('label-field')
			),
			'#weight' => 5,
		);
		

		return $form;
	}
	
	function content_sections_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
		$widget = $instance['widget'];
		$settings = $widget['settings'];
		
		$plugins = content_sections_section_types_options();
		
		
		
		$parents = array_merge($element['#field_parents'], array($element['#field_name'], $element['#language']));
		$id = drupal_html_id('content-sections-editor-'.implode('-',$parents));
		//ids are suffixed in ajax !?
		$id = preg_replace('/--\d+$/','',$id);
		
		content_sections_update_pos($parents,$form_state);
		
		//dpm(compact('form','form_state','field','instance','element','items','delta'));
		
		$element = array(
			'#type' => 'fieldset',
			'#tree' => TRUE,
			'#description' => NULL,
			'#collapsible' => isset($settings['container_collapsible'])?$settings['container_collapsible']:true,
			'#collapsed' => isset($settings['container_collapsed'])?$settings['container_collapsed']:false,
			'#id' => $id,
		) + $element;
		
		$element['sections'] = array(
			'#tree' => TRUE,
			'#theme' => 'content_sections_editor',
			'#mode' => '',
			'#attached' => array(
				'css' => array(),
				'js' => array(
					drupal_get_path('module', 'content_sections') . '/js/content_sections.js' => array('weight' => 10),
				),
			),
		);
		
		$sections = drupal_array_get_nested_value($form_state, array_merge(array('content_sections'),$parents),$key_exists);
		if(!$key_exists){
			$sections = content_sections_editor_load($items,$parents,$form,$form_state);
		}
		$edited = isset($sections['edited'])? $sections['edited']:false;
		if($edited !== false){
			$element['sections']['#mode'] = 'edit';
		}
		//dpm($sections);
		$nbSections = 0;
		if(!empty($sections)){
			$types = content_sections_section_types_options();
			foreach($sections as $key => $section){
				if(is_numeric($key) && empty($section['deleted'])){
					if($edited !== false && $key == $edited){
						$element['sections'][$key] = array(
							'label' => array(
								'#type' => 'textfield',
								'#title' => t('Title'),
								'#required' => true,
								'#default_value' => $section['label'],
								'#attributes' => array(
									'class' => array('label-field')
								),
							),
							'edit_form_container' => array(
								'#type' => 'container',
								'#parents' => array_merge($parents, array('sections',$key,'options')),
								'#attributes' => array(
									'class' => array('edit-form-container')
								),
							),
							'form_actions'=>array(
								'save' => array(
									'#type' => 'submit',
									'#name' => 'op_'.$id.'-'.$key,
									'#value' => t('Save'),
									'#limit_validation_errors'=> array($parents),
									'#ajax' => array(
										'callback' => 'content_sections_get_editor',
										'wrapper' => $id
									),
									'#content_sections_data_path' => array_merge($parents,array($key)),
									'#content_sections_editor_path' => $parents,
									'#submit' => array('content_sections_editor_edit_submit'),
								),
								'cancel' =>  array(
									'#type' => 'submit',
									'#name' => 'op_'.$id.'-'.$key,
									'#value' => t('Cancel'),
									'#limit_validation_errors'=> array($parents),
									'#ajax' => array(
										'callback' => 'content_sections_get_editor',
										'wrapper' => $id
									),
									'#content_sections_data_path' => array_merge($parents,array($key)),
									'#content_sections_editor_path' => $parents,
									'#submit' => array('content_sections_editor_cancel'),
								)
							)
						);
						$container = &$element['sections'][$key]['edit_form_container'];
						$plugin = $section['plugin'];
						content_sections_editor_section_form($container,$section,$form,$form_state);
						$form['#content_sections_to_submit'][] = array_merge($parents,array($key));
						if(!in_array('content_sections_check_to_submit',$form['#submit'])){
							$form['#submit'][] = 'content_sections_check_to_submit';
						}
					}else{
						$element['sections'][$key] = array(
							'title' => array(
								'#markup'=> check_plain($section['label']),
							),
							'type' => array(
								'#markup'=> check_plain($types[$section['plugin']]),
							),
							'actions' => array(
								'#type' => 'container',
								'#attributes' => array(
									'class' => array('actions')
								),
								'edit' => array(
									'#type' => 'submit',
									'#value' => t('Edit'),
									'#name' => 'op_'.$id.'-'.$key,
									'#limit_validation_errors'=> array($parents),
									'#ajax' => array(
										'callback' => 'content_sections_get_editor',
										'wrapper' => $id
									),
									'#content_sections_data_path' => array_merge($parents,array($key)),
									'#content_sections_editor_path' => $parents,
									'#submit' => array('content_sections_editor_edit'),
								),
								'delete'  => array(
									'#type' => 'submit',
									'#value' => t('Delete'),
									'#name' => 'op_'.$id.'-'.$key,
									'#limit_validation_errors'=> array($parents),
									'#ajax' => array(
										'callback' => 'content_sections_get_editor',
										'wrapper' => $id
									),
									'#content_sections_data_path' => array_merge($parents,array($key)),
									'#content_sections_editor_path' => $parents,
									'#submit' => array('content_sections_editor_delete'),
								),
							),
							'pos' => array(
								'delta' => array(
									'#title' => t('Delta'),
									'#type' => 'textfield',
									'#value' => $key,
									'#attributes' => array('class'=>array('delta-input')),
								),
								'weight' => array(
									'#title' => t('Weight'),
									'#type' => 'textfield',
									'#default_value' =>  isset($section['weight'])?$section['weight']:$nbSections,
									'#attributes' => array('class'=>array('weight-input')),
								),
								'parent_delta' => array(
									'#title' => t('Parent'),
									'#type' => 'textfield',
									'#default_value' => $section['parent_delta'],
									'#attributes' => array('class'=>array('parent-delta-input')),
								),
							)
						);
					}
					$element['sections'][$key]['#weight'] = isset($section['weight'])?$section['weight']:$nbSections;
					$element['sections'][$key]['#depth'] = isset($section['depth'])?$section['depth']:0;
					
					
					$nbSections++;
				}
			}
		}
		
		/*$element['update'] = array(
			'#type' => 'submit',
			'#value' => t('Update'),
			'#limit_validation_errors'=> array($parents),
			'#content_sections_editor_path' => $parents,
			'#submit' => array('content_sections_editor_test'),
		);*/
								
		
		if($edited === false){
			$plugin = drupal_array_get_nested_value($form_state,array_merge(array('values'),$parents,array('sections','new','plugin')));
			$add_mode = !empty($plugin);
			
			$element['sections']['new'] = array(
				'#weight' => isset($sections['new']['weight'])?$sections['new']['weight']:$nbSections,
				'#depth' => isset($sections['new']['depth'])?$sections['new']['depth']:0,
				'label' => array(
					'#type' => 'textfield',
					'#title' => t('Title'),
					'#required' => $add_mode,
					'#attributes' => array(
						'class' => array('label-field')
					),
				),
				'plugin' => array(
					'#type' => 'select',
					'#title' => t('Section type'),
					'#options' => $plugins,
					'#empty_option' => '- '.t('Select').' -',
					'#required' => $add_mode,
					'#attributes' => array(
						'class' => array('plugin-selector')
					),
					'#ajax' => array(
						'callback' => 'content_sections_editor_get_section_form',
						'wrapper' => $id.'-add'
					),
					'#content_sections_editor_path' => $parents,
				),
				'add_form_container' => array(
					'#type' => 'container',
					'#parents' => array_merge($parents, array('sections','new','options')),
					'#attributes' => array(
						'id' => $id.'-add',
						'class' => array('add-form-container')
					),
				),
				'actions' => array(
					'#attributes' => array(
						'class' => array('actions')
					),
					'add' => array(
						'#type' => 'submit',
						'#name' => 'op_'.$id.'-add',
						'#value' => t('Add section'),
						'#limit_validation_errors'=> array($parents),
						'#attributes' => array(
							'class' => array('button','submitAddSection'),
						),
						'#ajax' => array(
							'callback' => 'content_sections_get_editor',
							'wrapper' => $id
						),
						'#content_sections_editor_path' => $parents,
						'#submit' => array('content_sections_editor_add'),
					),
					'cancel' =>  array(
						'#type' => 'link',
						'#title' => t('Cancel'),
						'#href' => current_path(),
						'#options' => array(
							'attributes' => array(
								'class' => array('button','cancelSection'),
							),
							'html' => FALSE,
						),
					)
				),
				'pos' => array(
					'delta' => array(
						'#title' => t('Delta'),
						'#type' => 'textfield',
						'#value' => $nbSections,
						'#attributes' => array('class'=>array('delta-input')),
					),
					'weight' => array(
						'#title' => t('Weight'),
						'#type' => 'textfield',
						'#default_value' => isset($sections['new']['weight'])?$sections['new']['weight']:$nbSections,
						'#attributes' => array('class'=>array('weight-input')),
					),
					'parent_delta' => array(
						'#title' => t('Parent'),
						'#type' => 'textfield',
						'#default_value' => isset($sections['new']['parent_delta'])?$sections['new']['parent_delta']:'',
						'#attributes' => array('class'=>array('parent-delta-input')),
					),
				)
			);
			
			//dpm($element['sections']);
			
			//$element['sections']['new']['add_form_container']['debug']['#markup'] = 'wtf';
			//$element['sections']['new']['add_form_container']['debug']['#markup'] = kpr(compact('element'),true);
			//$element['sections']['new']['add_form_container']['debug']['#markup'] = kpr(compact('form','form_state','field','instance','element','items','delta'),true);
			
			//kpr($form_state['input']);
			if($add_mode){
				$plugin = $form_state['input'][$element['#field_name']][$element['#language']]['sections']['new']['plugin'];
				$container = &$element['sections']['new']['add_form_container'];
				
				content_sections_editor_section_form($container,array('plugin'=>$plugin),$form,$form_state);
				
			}
		}

		return $element;
	}
	
	function theme_content_sections_editor($vars){
		$form = $vars['form'];
		
		
		$id = 'content-sections-table-'.$form['#id'];
		$table = array(
			'attributes' => array(
				'id' => $id,
			)
		);
		$children = element_children($form);
		
		$table['header'][] = array(
			'data'=>(count($children) > 1?t('Title'):''),
		);
		if(count($children) > 1){
			$table['header'][] = array(
				'data'=>t('Type'),
			);
			$table['header'][] = array(
				'data'=>t('Operations'),
			);
		}
		if($form['#mode'] != 'edit') {
			$table['header'][] = array(
				'data'=>t('Position'),
			);
		}
		
		//kpr($form);
		
		foreach($children as $key){
			if(is_numeric($key)){
				if(!empty($form[$key]['edit_form_container'])){
					$table['rows'][] = array(
						'data' => array(array(
							'data' => array(
								$form[$key],
							),
							'colspan' => 3,
						))
					);
				}else{
					$row = array_merge($form[$key]['#attributes'], array(
						'data' => array(
							array(
								'data' => array(
									array(
										'#theme' => 'indentation',
										'#size' => $form[$key]['#depth'],
									),
									$form[$key]['title'],
								),
							),
							array(
								'data' => array(
									$form[$key]['type'],
								),
							),
							array(
								'data' => array(
									$form[$key]['actions'],
								),
							),
						)
					));
					if($form['#mode'] != 'edit') {
						$row['class'][] = 'draggable';
						$row['data'][] = array(
							'data' => array(
								$form[$key]['pos'],
							),
						);
					}
					$table['rows'][] = $row;
				}
			}elseif($key === 'new'){
				$btAdd = array(
					'#theme' => 'link',
					'#text' => t('Add section'),
					'#path' => current_path(),
					'#options' => array(
						'attributes' => array(
							'class' => array('button','addSection'),
						),
						'html' => FALSE,
					),
				);
				$row = array(
					'data' => array(
						array(
							'data' => array(
								array(
									'#theme' => 'indentation',
									'#size' => $form['new']['#depth'],
								),
								$btAdd,
							),
							'colspan' => count($children)>1?3:1,
						)
					),
					'id' => $id.'-add',
					'class' => array('new-row-row'),
				);
				if($form['#mode'] != 'edit') {
					$row['class'][] = 'draggable';
					$row['data'][] = array(
						'data' => array(
							$form['new']['pos'],
						),
					);
				}
				$table['rows'][] = $row;
				unset($form['new']['pos']);
				$table['rows'][] = array(
					'data' => array(array(
						'data' => array(
							$form['new'],
						),
						'colspan' => count($children)>1?4:2,
					)),
					'id' => $id.'-add',
					'class' => array('add-form-row'),
					'no_striping' => true,
				);
			}
		}
		
		if($form['#mode'] != 'edit') {
			// Add the tabledrag JavaScript.
			drupal_add_tabledrag($id, 'match', 'parent', 'parent-delta-input', 'parent-delta-input', 'delta-input', true, 4);
			drupal_add_tabledrag($id, 'order', 'sibling', 'weight-input', null, null, true);
		}
		
		//kpr($table);
		
		$form['#attributes']['class'][] = 'content-sections-editor';
		return '<div id="'.$form['#id'].'" '.drupal_attributes($form['#attributes']).'>'.theme('table__content_sections_editor',$table).'</div>';
	}
	function content_sections_editor_get_section_form($form,&$form_state){
		//return kpr($form,true);
		$out = '';
		$sectionFormPath = array_merge($form_state['triggering_element']['#content_sections_editor_path'],array('sections','new','add_form_container'));
		$sectionForm = drupal_array_get_nested_value($form,$sectionFormPath);
		$containerPath = $form_state['triggering_element']['#content_sections_editor_path'];
		$container = drupal_array_get_nested_value($form,$containerPath);
		//return 'wtf';
		//return kpr(compact('form','containerPath'),true);
		/*return kpr(array(
			ajax_command_replace('#'.$form_state['triggering_element']['#ajax']['wrapper'],render($sectionForm)),
			ajax_command_invoke('#'.$container['#id'],'trigger',array('SectionFormOpen')),
		),true);*/
		
		$container_id = $container['#id'];
		
		
		//return kpr(compact('container_id'),true);
		
		return array('#type' => 'ajax', '#commands' => array(
			ajax_command_insert(null,render($sectionForm)),
			ajax_command_invoke('#'.$container_id.' .content-sections-editor','trigger',array('SectionFormOpen')),
		));
	}
	function content_sections_editor_section_form(&$container,$section,$form,&$form_state){
		$container['#section_type'] = $section['plugin'];
		
		ctools_include('plugins');
		if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'form callback')){
			$container = $funct($container,$section,$form,$form_state);
		}
		if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'validate callback')){
			$container['#element_validate'][] = 'content_sections_editor_section_form_validate';
		}
	}
	function content_sections_editor_section_form_validate($form,&$form_state){
		ctools_include('plugins');
		if($funct = ctools_plugin_load_function('content_sections', 'section_type', $form['#section_type'], 'validate callback')){
			$funct($form,$form_state);
		}
	}
	function content_sections_get_editor($form,&$form_state){
		$containerPath = $form_state['triggering_element']['#content_sections_editor_path'];
		$container = drupal_array_get_nested_value($form,$containerPath);
		return $container;
	}
	
	function content_sections_update_pos($containerPath,&$form_state){
		$values = drupal_array_get_nested_value($form_state,array_merge(array('values'),$containerPath,array('sections')));
		if(!empty($values)){
			$posByParent = array();
			$positions = array();
			foreach($values as $delta => $val){
				if(!empty($val['pos'])){
					$positions[$delta === 'new'?'new':$val['pos']['delta']] = $val['pos'];
				}
			}
			if(!empty($positions)){
				$sections = drupal_array_get_nested_value($form_state,array_merge(array('content_sections'),$containerPath));
				unset($sections['edited']);
				foreach($sections as $key =>$section){
					if(array_key_exists($key,$positions)){
						$positions[$key] = array_merge($section,$positions[$key]);
					}else{
						$positions[$key] = $section;
					}
				}
				$positions = content_sections_update_tree_data($positions);
				//dpm($positions);
				$weights = array_flip(array_keys($positions));
				foreach($positions as $delta => $pos){
					$weight = $weights[$delta];
					drupal_array_set_nested_value($form_state,array_merge(array('values'),$containerPath,array('sections',$delta,'weight')),$weight);
					drupal_array_set_nested_value($form_state,array_merge(array('values'),$containerPath,array('sections',$delta,'parent_delta')),$pos['parent_delta']);
					drupal_array_set_nested_value($form_state,array_merge(array('content_sections'),$containerPath,array($delta,'weight')),$weight);
					drupal_array_set_nested_value($form_state,array_merge(array('content_sections'),$containerPath,array($delta,'parent_delta')),$pos['parent_delta']);
					drupal_array_set_nested_value($form_state,array_merge(array('content_sections'),$containerPath,array($delta,'depth')),$pos['depth']);
				}
			}
		}
	}
	
	
	function content_sections_editor_test($form,&$form_state){
		$containerPath = $form_state['triggering_element']['#content_sections_editor_path'];
		$form_state['rebuild'] = TRUE;
	}
	
	function content_sections_editor_load($items,$dataPath,$form,&$form_state){
		$sections = array();
		//dpm($items);
		foreach($items as $item){
			if($funct = ctools_plugin_load_function('content_sections', 'section_type', $item['plugin'], 'load callback')){
				$res = $funct($item,$form['#entity']->language);
				if(is_array($res)){
					$item = $res;
				}elseif($res===false){
					$item['deleted'] = true;
				}
			}
			$sections[] = $item;
		}
		drupal_array_set_nested_value($form_state, array_merge(array('content_sections'),$dataPath),$sections);
		return $sections;
	}
	function content_sections_editor_add($form,&$form_state){
		$containerPath = $form_state['triggering_element']['#content_sections_editor_path'];
		$container = drupal_array_get_nested_value($form,array_merge($containerPath,array('sections','new')));
		//dpm($form);
		//dpm($form_state);
		//$form_state['WTF'] = '?????';
		$form_state['rebuild'] = TRUE;
		//$form_state['content_sections'];
		$section = drupal_array_get_nested_value($form_state['values'],array_merge($containerPath,array('sections','new')));
		drupal_array_set_nested_value($form_state['values'],array_merge($containerPath,array('sections','new')),array());
		//dpm($section);
		if(!empty($section['plugin'] )){
			unset($section['actions']);
			$section['weight'] = $section['pos']['weight'];
			$section['parent_delta'] = $section['pos']['parent_delta'];
			unset($section['pos']);
			if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'data callback')){
				$res = $funct($section,null,$container,$form,$form_state);
				if(is_array($res)){
					$section = array_merge($section,$res);
				}elseif($res===false){
					$section['deleted'] = true;
				}
			}
			$sections = drupal_array_get_nested_value($form_state, array_merge(array('content_sections'),$containerPath));
			$sections[] = $section;
			unset($sections['new']);
			drupal_array_set_nested_value($form_state, array_merge(array('content_sections'),$containerPath),$sections);
			//dpm($form_state);
		}
		
		drupal_array_set_nested_value($form_state['values'],array_merge($containerPath,array('sections','new','plugin')),'');
	}
	
	function content_sections_editor_edit($form,&$form_state){
		$dataPath = $form_state['triggering_element']['#content_sections_data_path'];
		$pos = array_pop($dataPath);
		drupal_array_set_nested_value($form_state, array_merge(array('content_sections'),$dataPath,array('edited')),$pos);
		$form_state['rebuild'] = TRUE;
	}
	
	function content_sections_check_to_submit($form,&$form_state){
		//dpm($form['#content_sections_to_submit']);
		if(!empty($form['#content_sections_to_submit'])){
			foreach($form['#content_sections_to_submit'] as $dataPath){
				_content_sections_editor_edit_submit($dataPath,$form,$form_state);
			}
		}
	}
	
	function content_sections_editor_edit_submit($form,&$form_state){
		$dataPath = $form_state['triggering_element']['#content_sections_data_path'];
		_content_sections_editor_edit_submit($dataPath,$form,$form_state);
		
		_content_sections_editor_cancel($dataPath,$form,$form_state);
		$form_state['rebuild'] = TRUE;
	}
	
	function _content_sections_editor_edit_submit($dataPath,$form,&$form_state){
		$containerPath = $dataPath;
		$pos = array_pop($containerPath);
		$formPath = array_merge($containerPath, array('sections',$pos));
		$container = drupal_array_get_nested_value($form,$formPath);
		
		$original = drupal_array_get_nested_value($form_state, array_merge(array('content_sections'),$dataPath));
		$formValues = drupal_array_get_nested_value($form_state['values'],$formPath);
		unset($formValues['form_actions']);
		$section = array_merge($original,$formValues);
		//dpm($form_state);
		//dpm($formValues);
		//dpm($original);
		if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'data callback')){
			$res = $funct($formValues,$original,$container,$form,$form_state);
			if(is_array($res)){
				$section = array_merge($section,$res);
			}elseif($res===false){
				$section['deleted'] = true;
			}
		}
		//dpm($section);
		//dpm($dataPath);
		drupal_array_set_nested_value($form_state, array_merge(array('content_sections'),$dataPath),$section);
		
	}
	
	function content_sections_editor_delete($form,&$form_state){
		$dataPath = $form_state['triggering_element']['#content_sections_data_path'];
		drupal_array_set_nested_value($form_state, array_merge(array('content_sections'),$dataPath,array('deleted')),true);
		$form_state['rebuild'] = TRUE;
	}
	
	function content_sections_editor_cancel($form,&$form_state){
		$dataPath = $form_state['triggering_element']['#content_sections_editor_path'];
		_content_sections_editor_cancel($dataPath,$form,$form_state);
	}
	function _content_sections_editor_cancel($dataPath,$form,&$form_state){
		$containerPath = $dataPath;
		$sections = drupal_array_get_nested_value($form_state, array_merge(array('content_sections'),$containerPath));
		if(isset($sections['edited'])){
			unset($sections['edited']);
			drupal_array_set_nested_value($form_state, array_merge(array('content_sections'),$containerPath),$sections);
		}
		drupal_array_set_nested_value($form_state['values'],array_merge($containerPath,array('sections','new','plugin')),'');
		$form_state['rebuild'] = TRUE;
	}
		
	/**
	 * Implements hook_field_attach_submit().
	 */
	function content_sections_field_attach_submit($entity_type, $entity, $form, &$form_state) {
		list(, , $bundle_name) = entity_extract_ids($entity_type, $entity);

		//dpm($entity);
		foreach (field_info_instances($entity_type, $bundle_name) as $instance_name => $instance) {
			if (isset($instance['widget']) && strpos($instance['widget']['type'], 'content_sections') === 0) {
				$field_name = $instance['field_name'];
				$lang = $form[$field_name]['#language'];
				
				$sections = array();
				//dpm($form_state['content_sections'][$field_name]);
				//dpm($lang);
				if(!empty($form_state['content_sections'][$field_name][$lang])){
					content_sections_update_pos(array($field_name,$lang),$form_state);
					$allSections = $form_state['content_sections'][$field_name][$lang];
					//////// Sort by weight ////////
					uasort($allSections, 'drupal_sort_weight');
					//dpm($allSections);
					//////// Let plugins do operations before saving and remove deleted section ////////
					foreach($allSections as $delta => $section){
						if(is_numeric($delta)){
							if(isset($section['parent_delta']) && strlen($section['parent_delta']) && !empty($allSections[$section['parent_delta']]['deleted'])){
								$section['deleted'] = true;
							}
							if($funct = ctools_plugin_load_function('content_sections', 'section_type', $section['plugin'], 'save callback')){
								$res = $funct($section,$entity_type,$entity,$instance,$delta,$form,$form_state);
								if(is_array($res)){
									$section = $res;
								}elseif($res===false){
									$section['deleted'] = true;
								}
							}
							//dpm($section);
							if(!empty($section) && empty($section['deleted'])){
								$sections[$delta] = $section;
							}
						}
					}
					
				}
				//dpm($sections);
				$entity->{$field_name}[$lang] = $sections;
			}
		}
	}
	
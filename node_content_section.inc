<?php
	class NodeContentSection {
		var $contentType = null;
		
		function __construct($contentType) {
			$this->contentType = $contentType;
		}
   
		function form($container,$section,$form,&$form_state){
			//dpm($section);
			if(!empty($section['entity'])){
				$node = $section['entity'];
			}elseif(!empty($section['target'])){
				$node = $this->load($section,$form['#entity']->language);
			}else{
				$node = $this->_create($form['#entity']->language);
			}
			//dpm($node);
			field_attach_form('node', $node, $container, $form_state, $node->language);
			if(!empty($container['redirect'])){
				$container['redirect']['#access'] = false;
			}
			//dpm($container);
			//return kpr($container,true);
			
			return $container;
		}
		
		function validate($form,&$form_state){
			$node = $this->_create($form['#entity']->language);
			
			field_attach_form_validate('node', $node, $form, $form_state);
		}
		
		function submit($formValues,$section,$container,$form,&$form_state){
			if(empty($section)){
				$section = array();
			}
			if(!empty($section['entity'])){
				$node = $section['entity'];
			}elseif(!empty($section['target'])){
				$node = $this->load($section,$form['#entity']->language);
			}else{
				$node = $this->_create($form['#entity']->language,$formValues['options']);
			}
			$node->title = $formValues['label'];
			
			//dpm($container);
			$container = !empty($container['edit_form_container'])?$container['edit_form_container']:$container['add_form_container'];
			field_attach_submit('node', $node, $container, $form_state);
			
			//dpm($node);
			
			return array(
				'entity' => $node,
				'options' => array(),
			);
		}
		
		function load($section,$language=null){
			if(is_null($language)){
				$language = $GLOBALS['language']->language;
			}
			$nid = $section['target'];
			$translations = translation_node_get_translations($nid);
			if(!empty($translations[$language])){
				$nid = $translations[$language]->nid;
			}
			$node = null;
			if(!empty($section['target_rev'])){
				$node = node_load($nid,$section['target_rev']);
				if(empty($node)){ 
					$node = node_load($nid);
				}
			}else{
				$node = node_load($nid);
			}
			return $node;
		}
		
		function save($section,$entity_type,$entity,$instance,$delta,$form,&$form_state){
			if(empty($section['deleted'])){
				if(!empty($section['entity'])){
					$node = $section['entity'];
					$targetLang = !empty($entity->language)?$entity->language:'und';
					if($node->language != $targetLang){
						$this->translate($node,$targetLang);
					}
					$section['target_rev'] = null;
					if(!empty($entity->revision)){
						$node->revision = true;
						$this->updatePastRevisions($entity_type, $entity, $instance, $node);
						$section['target_rev'] = null;
					}
					node_save($node);
					unset($section['entity']);
					if(!empty($node->tnid)){
						$section['target'] = $node->tnid;
					}else{
						$section['target'] = $node->nid;
					}
					
				}
			}elseif(!empty($section['target'])){
				// make sure there was no problem where the section was used many times and delete it.
				$entity_type = $instance['entity_type'];
				$entity_id = entity_id($entity_type,$entity);
				
				$query = new EntityFieldQuery();
				$query->fieldCondition($instance['field_name'], 'target', $section['target']);
				$result = $query->execute();
				if(!empty($result[$entity_type][$entity_id])){
					unset($result[$entity_type][$entity_id]);
					if(empty($result[$entity_type])){
						unset($result[$entity_type]);
					}
				}
				if(empty($result)){
					node_delete($section['target']);
				}
			}
			return $section;
		}
		
		function updatePastRevisions($entity_type, $entity, $instance, $regionNode){
			if(!empty($regionNode->vid)){
				$field = field_info_field($instance['field_name']);
				if(!empty($field['storage']['details']['sql']['FIELD_LOAD_REVISION'])){
					$ids = entity_extract_ids($entity_type, $entity);
					$target_id = !empty($regionNode->tnid) ? $regionNode->tnid : $regionNode->nid;
					$columns = reset($field['storage']['details']['sql']['FIELD_LOAD_REVISION']);
					$rev_table = key($field['storage']['details']['sql']['FIELD_LOAD_REVISION']);
					db_update($rev_table)
					  ->fields(array(
						$columns['target_rev'] => $regionNode->vid,
					  ))
					  ->condition('entity_type', $entity_type)
					  ->condition('entity_id', $ids[0])
					  ->condition('revision_id', $ids[1], '<=')
					  ->condition($columns['target'], $target_id)
					  ->isNull($columns['target_rev'])
					  ->execute();
				}
			}
		}
		
		function translate($node,$language){
			if($node->language != $language && translation_supported_type($node->type)){
				if(empty($node->tnid)){
					$node->tnid = $node->nid;
				}
				if($node->language != 'und' || language_default('language') != $language){
					$original = node_load($node->tnid);
					if($original->language == 'und' || empty($original->tnid)){
						$original->tnid = $node->tnid;
						if($original->language == 'und'){
							$this->translate($original,language_default('language'));
						}
						$res = node_save($original);
					}
					unset($node->vid,$node->nid);
				}
				$info = array();
				foreach (field_info_instances('node', $node->type) as $field_name => $instance) {
					if(!empty($node->{$field_name})){
						$field = field_info_field($field_name);
						if($field['translatable']){
							$node->{$field_name} = array($language=>reset($node->{$field_name}));
						}
					}
				}
				$node->language = $language;
			}
		}
		
		function _create($language = NULL,$values=array()) {
			$default_values = array_merge(array(
				'type' =>$this->contentType,
				'language' => $language,
			),$values);
			return entity_create('node', $default_values);
		}
	}
?>
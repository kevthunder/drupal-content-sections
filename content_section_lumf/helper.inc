<?php 
class ContentSectionLumfHelper {
  function __construct() { 
    
  }
  
  function create_or_update($name,$options) {
    # deprecated, use this instead :
    return $this->save($name,$options);
  }
  
  function save($name,$options) {
    $defOpt = array(
      'label' => null,
      'field' => 'field_sections',
      'target' => null,
      'section' => array(),
      'parent' => null
    );
    $opt = array_merge($defOpt,$options);
    $section = lumf('node')->save($name,$opt['section']);
    $target_nid = is_numeric($opt['target'])? $opt['target'] : $opt['target']->nid;
    $translations = translation_node_get_translations($target_nid);
    if(empty($translations)) $translations = array((object)array('nid'=>$target_nid));
    foreach($translations as $trans){
      $target = node_load($trans->nid);
      if( !empty($section) && !empty($target) && property_exists($target, $opt['field']) ){
        if(!$this->findSection($target,$opt['field'],$section->nid)){
          if($plugin = $this->pluginFromNodeType($section->type)){
            if(is_null($opt['label'])) $opt['label'] = $section->title;
            $data = array(
              'label' => $opt['label'],
              'plugin' => $plugin,
              'target' => $section->nid,
            );
            if(!empty($opt['parent'])){
              $pnid = is_numeric($opt['parent'])?$opt['parent']:machinenamed_node_nid($opt['parent']);
              if($res = $this->findSection($target,$opt['field'],$pnid)){
                list($plang, $pindex) = $res;
                $data['parent_delta'] = $pindex;
              }
            }
            $target->{$opt['field']}[LANGUAGE_NONE][] = $data;
            node_save($target);
          }
        }
      }
    }
  }
  
  function pluginFromNodeType($nodeType){
    foreach (content_sections_get_section_types() as $plugin => $opt) {
      if($opt['node_type'] == $nodeType){
        return $plugin;
      }
    } 
  }
  
  function findSection($node,$field,$section_nid){
    foreach ($node->{$field} as $lang => $col) {
      foreach ($col as $index => $val) {
        if($val['target']==$section_nid){
          return array($lang,$index);
        }
      }
    }
    return false;
  }
  
}
 ?>